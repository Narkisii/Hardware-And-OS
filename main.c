#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUM_THREADS 4
#define HASH_SIZE 26

typedef struct { // hash table with 26 counters - one for every letter
    char letter;
    int count;
} HashTable;

HashTable letters_table[HASH_SIZE];

void initialize_hash(HashTable table[]) {
    for (int i = 0; i < HASH_SIZE; i++) {
        table[i].letter = 'a' + i;
        table[i].count = 0;
    }
}

void count_letters(HashTable table[], char c) {
    if (c >= 'A' && c <= 'Z') {
        c += 'a' - 'A'; // Convert to lowercase
    }
    if (c >= 'a' && c <= 'z') {
        table[c - 'a'].count++;
    }
}

void print_hash(HashTable table[]) {
    for (int i = 0; i < HASH_SIZE; i++) {
        printf("%c: %d\n", table[i].letter, table[i].count);
    }
}

FILE *open_file(void *file_path) {
    char *path = (char *) file_path;
    printf("Thread #%s is reading the file.\n", path);

    FILE *file;
    file = fopen(path, "r");

    // check if file opened successfully or not & return message to screen
    if (file == NULL) {
        printf("Thread #%s failed to open the file.\n", path);
        pthread_exit(NULL);
    }
    printf("Successfully opened the file: %s\n", (char *) file_path);
    return file;
}

void add_counts_to_main(HashTable table[]) {
    for(int i = 0; i < HASH_SIZE; i++) {
        letters_table[i].count += table[i].count;
    }
}

void *run_thread(void *args) {
    HashTable td_letters[HASH_SIZE];
    FILE *file;

    file = open_file(args); // open the file
    initialize_hash(td_letters); // initialize hash table with counters for every letter

    char ch;
    // read the file and count every letter
    // (will count lower case and upper case the same)
    while ((ch = fgetc(file)) != EOF) {
        count_letters(td_letters, ch);
    }
    add_counts_to_main(td_letters);

    fclose(file);
    pthread_exit(NULL);
}

int main() {
    int pread;
    initialize_hash(letters_table);

    pthread_t threads[NUM_THREADS];
    char *files[NUM_THREADS] = {"file1.txt", "file2.txt", "file3.txt", "file4.txt"};

    for (int t = 0; t < NUM_THREADS; t++) {
        printf("Creating thread to read file: %s\n", files[t]);
        pread = pthread_create(&threads[t], NULL, run_thread, (void *) files[t]);
        if (pread != 0) {
            printf("ERROR; return code from pthread_create() is %d\n", pread != 0);
            exit(-1);
        }
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }
    print_hash(letters_table);

    pthread_exit(NULL);
    //return 0;
}
