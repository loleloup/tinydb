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
    char query[256];
    database_t db;
    printf("%zu\n", db.lsize);

    printf("Good luck in this projet!\n");

    while (fgets(query, 256, stdin) != NULL){
        printf("input read : %s\n", query);

    }

    db_load(&db, "/home/me/CLionProjects/tinydb/students.bin");
    db_save(&db, "/home/me/CLionProjects/tinydb/save_test.bin");

    return 0;
}
