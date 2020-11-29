#define __USE_XOPEN
#define _GNU_SOURCE

#include "student.c"
#include "db.c"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int read_file(FILE* file){
    char* line;
    while ((line = fgets("\n", 100, file)) != NULL){
        printf("%s", line);
    }
    return 0;
}


int main(int argc, char const **argv) {
    database_t db;
    printf("%zu\n", db.lsize);
    //int running = 1;

    printf("Good luck in this projet!\n");



    db_load(&db, "/home/me/CLionProjects/tinydb/students.bin");

    return 0;
}


