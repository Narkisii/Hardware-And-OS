#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4

/*
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; // Setting Up MUTEX

struct arg_struct { // Setting Up Struct For Mapping Argument. For Ranging (To Split Work For Threading)
    int arg1; // First Argument, From.
    int arg2; // Second Argument, To.
}; */


void *read_file(void *file_path) {
    //long tid;
    //tid = (long) file_path;
    char *path = (char *) file_path;
    printf("Thread #%s is reading the file.\n", path);

    FILE *file;
    file = fopen(path, "r");

    // check if file opened successfully or not & return message to screen
    if (file == NULL) {
        printf("Thread #%s failed to open the file.\n", path);
        pthread_exit(NULL);
    }
    printf("Successfully opened the file: %s\n", file_path);

    char ch;
    while ((ch = fgetc(file)) != EOF) {
        printf("%c", ch);
    }

    fclose(file);
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    int pread;

    char *files[NUM_THREADS] = {"file1.txt", "file2.txt", "file3.txt", "file4.txt"};


    pread = pthread_create(&threads[0], NULL, read_file, (void *) files[0]);
    if (pread != 0) {
        printf("ERROR; return code from pthread_create() is %d\n", pread);
        exit(-1);
    }

    int join = pthread_join(threads[0], NULL); // Join to Get Words Into Test Array
    if(join != 0) { // In Case Thread Join Fails
        fprintf(stderr, "Join Failed");
        return 1;
    }

    /*
     for(int t = 0; t < NUM_THREADS; t++) {
     printf("Creating thread to read file: %s\n", files[t]);
     rc = pthread_create(&threads[t], NULL, read_file, (void *)files[t]);
         if (rc) {
             printf("ERROR; return code from pthread_create() is %d\n", rc);
             exit(-1);
         }
    }
    */

    pthread_exit(NULL);
    //return 0;
}
