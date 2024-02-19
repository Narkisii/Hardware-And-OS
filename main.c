#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define NUM_THREADS 4
#define HASH_SIZE 26

/*
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; // Setting Up MUTEX

struct arg_struct { // Setting Up Struct For Mapping Argument. For Ranging (To Split Work For Threading)
    int arg1; // First Argument, From.
    int arg2; // Second Argument, To.
}; */

typedef struct { // hash table with 26 counters - one for every letter
    char letter;
    int count;
} HashTable;

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

void read_file(FILE *file) {
    char ch;
    while ((ch = fgetc(file)) != EOF) {
        //printf("%c", ch);

    }
    fclose(file);
    pthread_exit(NULL);
}

void *run_thread(void *args) {
    HashTable td_letters[HASH_SIZE];
    FILE *file;

    file = open_file(args); // open the file
    initialize_hash(td_letters); // initialize hash table with counters for every letter

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        count_letters(td_letters, ch);
    }
    print_hash(td_letters);

    fclose(file);
    pthread_exit(NULL);
}

int main() {
    int pread;
    HashTable lettersTable[HASH_SIZE];
    initialize_hash(lettersTable);

    pthread_t threads[NUM_THREADS];
    char *files[NUM_THREADS] = {"file1.txt", "file2.txt", "file3.txt", "file4.txt"};

    /*
    pread = pthread_create(&threads[0], NULL, run_thread, (void *) files[0]);
    if (pread != 0) {
        printf("ERROR; return code from pthread_create() is %d\n", pread);
        exit(-1);
    }
     */

    for (int t = 0; t < NUM_THREADS; t++) {
        printf("Creating thread to read file: %s\n", files[t]);
        pread = pthread_create(&threads[t], NULL, run_thread, (void *) files[t]);
        if (pread != 0) {
            printf("ERROR; return code from pthread_create() is %d\n", pread != 0);
            exit(-1);
        }
    }
    /*
 int join = pthread_join(threads[0], NULL); // Join to Get Words Into Test Array
 if (join != 0) { // In Case Thread Join Fails
     fprintf(stderr, "Join Failed");
     return 1;
 } */
    pthread_exit(NULL);
    //return 0;
}
