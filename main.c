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

#define MAX_THREAD 4

sem_t select_array;
sem_t active_threads;
pthread_t active_select[4];
pthread_t active_modifier;

database_t db;



void *insert_thread(void *arg){
    student_t *s = (student_t*) arg;
    db_add(&db, *s);
    sem_post(&select_array);
    sem_post(&active_threads);
    pthread_exit(NULL);
    return NULL;
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

    return 0;
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
    student_t s;

    printf("Good luck in this projet!\n");

    db_init(&db);


    while (fgets(query, 256, stdin) != NULL){
        //printf("input read : %s\n", query);
        strtok_r(query, " ", &rest);
        if (strcmp(query, "insert") == 0){  //insert query
            if (parse_insert(rest, (char *)&s.fname, (char *)&s.lname, &s.id, (char *)&s.section, &s.birthdate)){   //parse OK
                if (sem_wait(&active_threads) == 0) { //se met en attente si il y a deja trop de thread actifs
                    if (sem_wait(&select_array) == 0) { //Bloque nouvelles queries durant exécution

                        pthread_create(&active_modifier, NULL, insert_thread, &s);
                    }
                    else{
                        printf("error when waiting for A_AR");
                    }
                }
                else{
                    printf("error when waiting for A_TH");
                }
            }
        }

        else if(strcmp(query, "delete") == 0){
            //delete_command(rest, &db);
        }
        else if(strcmp(query, "update") == 0){
            //update_command(rest, &db);
        }
        else if(strcmp(query, "select") == 0){
            //select_command(rest, &db);
        }


    }

    db_load(&db, "/home/me/CLionProjects/tinydb/students.bin");
    db_save(&db, "/home/me/CLionProjects/tinydb/save_test.bin");

    return 0;
}
