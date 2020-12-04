#define __USE_XOPEN
#define _GNU_SOURCE

#include "student.c"
#include "db.c"
#include "parsing.c"

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

sem_t select_array;
sem_t active_thread;
pthread_t active_select[8];

int insert_command(char *query){
    char *fname = malloc(64);
    char *lname = malloc(64);
    char *section = malloc(64);

    unsigned ID;
    struct tm birthdate;

    if (parse_insert(query, fname, lname, &ID, section, &birthdate)){
        printf("insert parsing passed\n");

    }
}


int update_command(char* query){
    char* field_filter = malloc(64);
    char* value_filter = malloc(64);
    char* field_to_update = malloc(64);
    char* update_value = malloc(64);

    if (parse_update(query, field_filter, value_filter, field_to_update, update_value)){
        printf("update parsing passed\n");
    }
}

int select_command(char *query, database_t *db){
    int i;
    char *field = malloc(64);
    char *value = malloc(64);

    if (parse_selectors(query, field, value)){
        printf("select parsing passed\n");
        if (sem_wait(&active_thread) == 0){
            if (sem_wait(&select_array) == 0) {
                i = 0;
                while (active_select[i] != 0) i++;
                pthread_create(&active_select[i], NULL, db_select(db, field, value), NULL);
            }
        }
    }
}


int delete_command(char *query){
    char *field = malloc(64);
    char *value = malloc(64);

    if (parse_selectors(query, field, value)){
        printf("delete parsing passed\n");

    }
}


int main(int argc, char const **argv) {
    char query[256];
    char *rest;
    char command[256];
    database_t db;
    sem_init(&select_array, 0, 1);
    sem_init(&active_thread, 0, 8);

    printf("Good luck in this projet!\n");

    while (fgets(query, 256, stdin) != NULL){
        //printf("input read : %s\n", query);
        strtok_r(query, " ", &rest);
        if (strcmp(query, "insert") == 0){
            insert_command(rest);
        }
        else if(strcmp(query, "delete") == 0){
            delete_command(rest);
        }
        else if(strcmp(query, "update") == 0){
            update_command(rest);
        }
        else if(strcmp(query, "select") == 0){
            select_command(rest, &db);
        }


    }

    db_load(&db, "/home/me/CLionProjects/tinydb/students.bin");
    db_save(&db, "/home/me/CLionProjects/tinydb/save_test.bin");

    return 0;
}
