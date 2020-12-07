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

sem_t selector_db_sem;
sem_t insert_struct;
pthread_t threads[4];
database_t db;
database_t selector_db;

struct select_args{
    int i_start;
    int i_end;
    char *field;
    char *value;
};

static void wait_threads(int sig){
    printf("sigint\n");
    kill(0, SIGTERM);
    return;
}

void *select_thread(void *_args){
    struct select_args *args = (struct select_args *) _args;    //converts void* back to struct
    int i;
    student_t s;
    //printf("value = %s, field = %s, i_start = %i, i_end = %i\n", args->value, args->field, args->i_start, args->i_end);

    if (strcmp(args->field, "ID") == 0) {
        for (i = args->i_start; i <= args->i_end; ++i){
            s = db.data[i];
            if (s.id == (unsigned) atol(args->value)){
                sem_wait(&selector_db_sem);
                db_add(&selector_db, s);
                sem_post(&selector_db_sem);
            }}}

    else if (strcmp(args->field, "fname")==0) {
        for (i = args->i_start; i <= args->i_end; ++i){
            s = db.data[i];
            if (strcmp(args->value, s.fname) == 0){
                sem_wait(&selector_db_sem);
                db_add(&selector_db, s);
                sem_post(&selector_db_sem);
            }}}

    else if (strcmp(args->field, "lname")==0) {
        for (i = args->i_start; i <= args->i_end; ++i){
            s = db.data[i];
            if (strcmp(args->value, s.lname) == 0) {
                sem_wait(&selector_db_sem);
                db_add(&selector_db, s);
                sem_post(&selector_db_sem);
            }}}

    else if (strcmp(args->field, "section")==0) {

        for (i = args->i_start; i <= args->i_end; ++i){
            s = db.data[i];
            if (strcmp(args->value, s.section) == 0) {
                sem_wait(&selector_db_sem);
                db_add(&selector_db, s);
                sem_post(&selector_db_sem);
            }}}

    else if (strcmp(args->field, "birthdate")==0) {
        struct tm *date;
        time_t base;
        time_t stud;
        strptime(args->value, "%d/%m/%Y", date);

        base = mktime(date);
        for (i = args->i_start; i <= args->i_end; ++i){
            s = db.data[i];
            stud = mktime(&s.birthdate);
            if (base == stud){
                sem_wait(&selector_db_sem);
                db_add(&selector_db, s);
                sem_post(&selector_db_sem);
            }}}

    return NULL;



}


void *select_command(char *field, char *value, struct select_args *selectors){
    int i;

    //initialise la db qui va contenir les résultats
    db_empty(&selector_db);

    //divide db in 4 sections and create struct for each thread
    int step = db.lsize/4;
    for (i=1; i<4;++i){
        selectors[i].i_start = (step*i) + 1;
    }
    for (i=0; i<4;++i){
        selectors[i].value = value;
        selectors[i].field = field;
        selectors[i].i_end = step*(i+1);
    }
    if (db.lsize%2 == 0){selectors[3].i_end += 1;}

    //create threads
    for (i=0; i<4;++i){
        pthread_create(&threads[i], NULL, select_thread, &selectors[i]);
    }
    //wait for threads to finish
    for (i=0; i<4;++i){
        pthread_join(threads[i], NULL);
    }

}




int main(int argc, char const **argv) {
    char query[256];
    char *rest;

    char field[64];
    char value[64];

    int i;

    struct select_args selectors[4];
    selectors[0].i_start = 0; //par défaut

    student_t s;

    signal(SIGINT, wait_threads);
    db_init(&db);


    sem_init(&selector_db_sem, 0, 1);





    while (fgets(query, 256, stdin) != NULL){
        printf("input read : %s\n", query);
        strtok_r(query, " ", &rest);
        if (strcmp(query, "insert") == 0){  //insert query
            if (parse_insert(rest, (char *)&s.fname, (char *)&s.lname, &s.id, (char *)&s.section, &s.birthdate)){   //parse OK
                clock_t begin = clock();

                db_add(&db, s);

                clock_t end = clock();
                double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
                //printf("%f\n", time_spent);

            }
        }

        else if(strcmp(query, "delete") == 0){
            if (parse_selectors(rest, field, value)){   //parse OK

                clock_t begin = clock();

                select_command(field, value, selectors);    //sélectionne les étudiants

                for (i=0;i<selector_db.lsize; ++i){
                    s = selector_db.data[i];

                }
                clock_t end = clock();
                double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
                printf("%f\n", time_spent);
            }



        }
        else if(strcmp(query, "update") == 0){

        }
        else if(strcmp(query, "select") == 0){
            if (parse_selectors(rest, field, value)){   //parse OK

                clock_t begin = clock();

                select_command(field, value, selectors);

                clock_t end = clock();
                double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
                printf("%f\n", time_spent);

                //printf("selector size = %zu\n", selector_db.lsize);
                for (i=0; i<selector_db.lsize; ++i){
                    student_to_str(rest, &selector_db.data[i]);
                    printf("%s\n", rest);
                }


            }
        }


    }
    printf("input read\n");
    printf("%i\n", selector_db.lsize);
    for (i=0;i<selector_db.lsize; ++i){
        s = selector_db.data[i];
        db_delete(&db, &s);
    }

    for (i=0;i<db.lsize; ++i){
        s = db.data[i];
        student_to_str(rest, &s);
        printf("%s\n", rest);

    }


    return 0;
}
