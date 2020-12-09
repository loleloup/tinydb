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
#define true 1
#define false 0

sem_t selector_db_sem;
sem_t insert_struct;
pthread_t threads[4];
database_t db;
database_t selector_db;
int running = true;


struct select_args{
    int i_start;
    int i_end;
    char *field;
    char *value;
};


static void wait_threads(int sig){

    running = false;
    printf("waiting for request to terminate\n");
    return;
}


void *select_thread(void *_args){
    struct select_args *args = (struct select_args *) _args;    //converts void* back to struct
    int i;
    student_t s;
    printf("value = %s, field = %s, i_start = %i, i_end = %i\n", args->value, args->field, args->i_start, args->i_end);

    if (strcmp(args->field, "id") == 0) {
        for (i = args->i_start; i <= args->i_end; ++i){
            s = db.data[i];

            if (s.id == (unsigned) strtoul(args->value, NULL, 10)){

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
        struct tm date;
        time_t base;
        time_t stud;
        strptime(args->value, "%d/%m/%Y", &date);
        base = mktime(&date);

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
    return NULL;
}


void *update_thread(void *_args){
    struct select_args *args = (struct select_args *) _args;    //converts void* back to struct
    int i;
    student_t old;
    //printf("value = %s, field = %s, i_start = %i, i_end = %i\n", args->value, args->field, args->i_start, args->i_end);

    if (strcmp(args->field, "id") == 0) {
        for (i = args->i_start; i <= args->i_end; ++i) {
            old = selector_db.data[i];
            selector_db.data[i].id = (unsigned) strtoul(args->value, NULL, 10);
            db_update(&db, &old, &selector_db.data[i]);
        }}

    else if (strcmp(args->field, "fname")==0) {
        for (i = args->i_start; i <= args->i_end; ++i) {
            old = selector_db.data[i];
            strcpy(selector_db.data[i].fname, args->value);
            db_update(&db, &old, &selector_db.data[i]);
        }}

    else if (strcmp(args->field, "lname")==0) {
        for (i = args->i_start; i <= args->i_end; ++i) {
            old = selector_db.data[i];

            strcpy(selector_db.data[i].lname, args->value);
            db_update(&db, &old, &selector_db.data[i]);
        }}

    else if (strcmp(args->field, "section")==0) {
        for (i = args->i_start; i <= args->i_end; ++i) {
            old = selector_db.data[i];
            strcpy(selector_db.data[i].section, args->value);
            db_update(&db, &old, &selector_db.data[i]);
        }}

    else if (strcmp(args->field, "birthdate")==0) {
        struct tm date;
        strptime(args->value, "%d/%m/%Y", &date);

        for (i = args->i_start; i <= args->i_end; ++i) {
            old = selector_db.data[i];
            selector_db.data[i].birthdate = date;
            db_update(&db, &old, &selector_db.data[i]);
        }}

    return NULL;
}



void *update_command(char *field, char *value, struct select_args *selectors){
    int i;
    if (selector_db.lsize > 1) {
        int step = selector_db.lsize / 4;
        for (i = 1; i < 4; ++i) {
            selectors[i].i_start = (step * i) + 1;
        }
        for (i = 0; i < 4; ++i) {
            selectors[i].value = value;
            selectors[i].field = field;
            selectors[i].i_end = step * (i + 1);
        }
        if (selector_db.lsize % 2 == 0) { selectors[3].i_end += 1; }

        for (i = 0; i < 4; ++i) {
            pthread_create(&threads[i], NULL, update_thread, &selectors[i]);
        }
        //wait for threads to finish
        for (i = 0; i < 4; ++i) {
            pthread_join(threads[i], NULL);
        }
    }
    else{
        selectors[0].i_start = 0;
        selectors[0].i_end = 0;
        selectors[0].value = value;
        selectors[0].field = field;
        update_thread(&selectors[0]);
    }
    return NULL;
}


int main(int argc, char const **argv) {
    char query[256];
    char *rest;
    char field[64];
    char value[64];
    char field_filter[64];
    char value_filter[64];
    char logspath[286];
    FILE *log;
    int i;
    student_t s;
    struct select_args selectors[4];
    db_init(&db);
    sem_init(&selector_db_sem, 0, 1);
    selectors[0].i_start = 0; //par défaut

    signal(SIGINT, wait_threads);

    if (argc > 1){
        db_load(&db, argv[1]);
    }

    while (fgets(query, 256, stdin) != NULL && running){
        strtok_r(query, " ", &rest);
        rest = strtok_r(NULL, "\n", &rest); //strip le \n
        clock_t begin = clock();    //start clock

        sprintf(logspath, "logs/%ld-%s.txt", time(NULL), query);
        log = fopen(logspath, "w");
        fprintf(log, "Query \"%s %s\" completed in ", query, rest);

        if (strcmp(query, "insert") == 0){  //insert query
            if (parse_insert(rest, (char *)&s.fname, (char *)&s.lname, &s.id, (char *)&s.section, &s.birthdate)){   //parse OK

                db_add(&db, s);


                //printf("%f\n", time_spent);

            }
        }

        else if(strcmp(query, "delete") == 0){
            if (parse_selectors(rest, field, value)){   //parse OK

                select_command(field, value, selectors);    //sélectionne les étudiants

                for (i=0;i<selector_db.lsize; ++i){
                    s = selector_db.data[i];
                    db_delete(&db, &s);
                }

            }
        }

        else if(strcmp(query, "update") == 0){
            if (parse_update(rest, field_filter, value_filter, field, value)) {   //parse OK

                select_command(field_filter, value_filter, selectors);

                update_command(field, value, selectors);


            }
        }

        else if(strcmp(query, "select") == 0){
            if (parse_selectors(rest, field, value)){   //parse OK

                select_command(field, value, selectors);

            }
        }

        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        fprintf(log, "%f ms with %li results\n", time_spent, selector_db.lsize);
        for (i=0; i<selector_db.lsize; ++i){
            student_to_str(rest, &selector_db.data[i]);
            fprintf(log, "%s\n", rest);
        }
        //fprintf(log, "\n");
        fclose(log);
    }


    if (argc > 1){
        db_save(&db, argv[1]);
    }
    else {
        db_save(&db, "temp_db");
    }

    return 0;
}
