#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define NUM_THREADS 4
#define HASH_SIZE 26

typedef struct { // hashtable structure
    char letter;
    int count;
} HashTable;

typedef enum { // enum types for handling errors
    FILE_NOT_EXIST = 1,
    FILE_OPEN_FAILED,
    ALLOCATION_ERROR,
    FILE_READ_ERROR,
    THREAD_CREATION_ERROR
} ErrorCode;

HashTable letters_table[HASH_SIZE]; // hash table for the main thread - counts the total number of letters

/* ------------
 * Initialize a new hash table. Every cell holds:
 * KEY: An alphabetic letter (in lower case).
 * VALUE: Counter that counts the number of times the letter appeared in a text.
 */
void initialize_hash(HashTable table[]) {
    for (int i = 0; i < HASH_SIZE; i++) {
        table[i].letter = 'a' + i;
        table[i].count = 0; // initialize all counters to 0
    }
}

/* ------------
 * Function goal: Promote the matching letter's counter when the letter is alphabetic.
 */
void count_letters(HashTable table[], char c) {
    // check if c is upper case and convert it to lower case
    if (c >= 'A' && c <= 'Z') {
        c += 'a' - 'A';
    }
    // check if c is a alphabetic and update the counter in hash table
    if (c >= 'a' && c <= 'z') {
        table[c - 'a'].count++;
    }
}

/* ------------
 * Helping method to print the hash table
 */
void print_hash(HashTable table[]) {
    for (int i = 0; i < HASH_SIZE; i++) {
        printf("%c: %d\n", table[i].letter, table[i].count);
    }
}

/* ------------
 * Function goal: Add the counts from every thread to the main thread hash table
 */
void add_counts_to_main(HashTable table[]) {
    for (int i = 0; i < HASH_SIZE; i++) {
        letters_table[i].count += table[i].count;
    }
}

/* ------------
 * Function goal: Print the relevant error message by given error type
 */
void print_error_message(int err_type, int tid) {
    switch (err_type) {
        case FILE_NOT_EXIST:
            printf("Error in thread #%d: The file does not exist.\n", tid);
            break;
        case FILE_OPEN_FAILED:
            printf("Error in thread #%d: Failed to open the file.\n", tid);
            break;
        case ALLOCATION_ERROR:
            printf("Error in thread #%d: Failed to allocate the memory required.\n", tid);
            break;
        case FILE_READ_ERROR:
            printf("Error in thread #%d: Encountered an error reading the file.\n", tid);
            break;
        case THREAD_CREATION_ERROR:
            printf("Error in thread #%d: Failed to create the thread.\n", tid);
            break;
        default:
            printf("An unknown error occurred in thread #%d.\n", tid);
    }
}

/* ------------
 * Function goal: Check if a file exist or not.
 * If exists, return true. False otherwise.
 */
bool is_exist(char *file_path) {
    int is_exists = access(file_path, F_OK);
    if (is_exists == 0)
        return true;
    return false;
}

/* ------------
 * Main function that run the threads commands
 */
void *run_thread(void *file_path) {
    char *path = (char *) file_path; // convert the file path to string

    // Handles error: File is not exist
    bool exists = is_exist(path);
    if (exists == false) {
        pthread_exit((void *) FILE_NOT_EXIST);
    } else {
        FILE *file = fopen(path, "r"); // Open the file using the path

        if (file == NULL) { // Handles error: File didn't open properly
            pthread_exit((void *) FILE_OPEN_FAILED);
        }
        // Allocate memory for the hash table
        HashTable *td_letters = malloc(HASH_SIZE * sizeof(HashTable));
        if (td_letters == NULL) { // Handles error: Memory allocation failed
            fclose(file);
            pthread_exit((void *) ALLOCATION_ERROR);
        }
        initialize_hash(td_letters);

        char ch;
        while ((ch = fgetc(file)) != EOF) { // read file
            if (ferror(file)) { // Handles errors while reading the file
                free(td_letters);
                fclose(file);
                pthread_exit((void *) FILE_READ_ERROR);
            }
            count_letters(td_letters, ch);
        }
        add_counts_to_main(td_letters); // add the data this thread collected to the main thread counting
        free(td_letters);
        fclose(file); // close file
    }
    pthread_exit(NULL); // Will only execute if the thread completed properly
}

// ____________________
// ____________________
int main() {
    int p_read;
    ErrorCode err = 0; // in case of an error, it will hold the error code
    void *status;
    initialize_hash(letters_table); // initialize the hash table for main thread
    pthread_t threads[NUM_THREADS];
    char *files[NUM_THREADS] = {"file1.txt", "file2.txt", "file3.txt", "file4.txt"};

    for (int t = 0; t < NUM_THREADS; t++) { // create 4 threads
        p_read = pthread_create(&threads[t], NULL, run_thread, (void *) files[t]);
        // Handles errors in the thread creating
        if (p_read != 0) {
            printf("ERROR; return code from pthread_create() is %d\n", p_read != 0);
            err = THREAD_CREATION_ERROR;
            break;
        }
    }
    for (int t = 0; t < NUM_THREADS; t++) { // join the threads after finish
        pthread_join(threads[t], &status);
        if(status != NULL) {
            err = (ErrorCode)status;
            print_error_message(err, t+1);
        }
    }
    printf("Letter frequencies in all files:\n");
    print_hash(letters_table);

    pthread_exit(NULL);
}
