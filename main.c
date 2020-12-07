#define __USE_XOPEN
#define _GNU_SOURCE

#include "student.c"
#include "db.c"
#include "parsing.c"

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>

#define MAX_THREAD 4

sem_t semaphores[4];
sem_t insert_struct;
pthread_t threads[4];
database_t db;

struct insert_args{
    int i;
    student_t *s;
};

struct select_args{
    int i;
    char *field;
    char *value;
} *select_args;

static void wait_threads(int sig){
    printf("sigint\n");
    kill(0, SIGTERM);
    return;
}

void *insert_thread(void *args){
    printf("here");
    struct insert_args *arg = (struct insert_args *)args;
    printf("here2");


    char buffer[64];
    student_to_str((char *)buffer, arg->s);
    printf("thread %lu, index %i, student : %s\n", threads[arg->i], arg->i, buffer);
    sem_wait(&semaphores[arg->i]);
    db_add(&dbs[arg->i], *arg->s);
    sem_post(&semaphores[arg->i]);
    free(args);
    return NULL;
}


void *update_command(void *args){

}

void *select_thread(void *args){

    struct select_args *arg = (struct select_args *)args;

    sem_wait(&semaphores[arg->i]);
    db_select(&dbs[arg->i], arg->field, arg->value);
    sem_post(&semaphores[arg->i]);
    return NULL;



}


void *delete_command(void *args){

}


int main(int argc, char const **argv) {
    char query[256];
    char *rest;

    char field[64];
    char value[64];

    int i;
    int db_i;
    int min_lsize;
    student_t s;

    struct insert_args *insert_ptr;

    signal(SIGINT, wait_threads);

    for (i = 0; i<MAX_THREAD; ++i){ //init les db
        db_init(&dbs[i]);
    }

    for (i = 0; i<MAX_THREAD; ++i){ //init les semaphores
        sem_init(&semaphores[i], 0, 1);
    }


    select_args = malloc(sizeof(struct select_args));


    while (fgets(query, 256, stdin) != NULL){
        //printf("input read : %s\n", query);
        strtok_r(query, " ", &rest);
        if (strcmp(query, "insert") == 0){  //insert query
            if (parse_insert(rest, (char *)&s.fname, (char *)&s.lname, &s.id, (char *)&s.section, &s.birthdate)){   //parse OK
                min_lsize = dbs[0].lsize;
                db_i = 0;
                for (i=1; i<MAX_THREAD; ++i){   //choisi dans quelle db ajouter le student
                    if (dbs[i].lsize <= min_lsize){
                        min_lsize = dbs[i].lsize;
                        db_i = i;
                    }
                }
                insert_ptr = (struct insert_args *) malloc(sizeof(struct insert_args));

                insert_ptr->i = db_i;
                insert_ptr->s = &s;


                printf("ptr = %p\n", insert_ptr);

                student_to_str(rest, &s);
                printf("created thread %lu, index %i, student : %s\n", threads[db_i], db_i, rest);
                pthread_create(&threads[db_i], NULL, insert_thread, &insert_ptr);


            }
        }

        else if(strcmp(query, "delete") == 0){
            //delete_command(rest, &db);
        }
        else if(strcmp(query, "update") == 0){
            //update_command(rest, &db);
        }
        else if(strcmp(query, "select") == 0){
            if (parse_selectors(rest, field, value)){   //parse OK
                for (i=0; i<MAX_THREAD; ++i){   //choisi dans quelle db ajouter le student
                    select_args->i = db_i;
                    select_args->field = field;
                    select_args->value = value;
                    pthread_create(&threads[db_i], NULL, select_thread, select_args);
                }



            }
        }


    }

    for (i=0; i<MAX_THREAD; ++i){
        pthread_join(threads[i], NULL);
    }

    int j;
    for (i=0; i<MAX_THREAD; ++i){
        for (j=0; j<dbs[i].lsize; ++j){
            student_to_str(rest, &dbs[i].data[j]);
            printf("%s\n", rest);
        }
    }


    return 0;
}
