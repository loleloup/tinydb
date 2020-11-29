#include "db.h"
#include "student.h"

#include <stdlib.h>

void db_add(database_t *db, student_t s){
    int i;
    printf("adding ");
    if (db->lsize == db->psize/sizeof(student_t)){
        db->psize *= 2;
    }

    student_t *old = db->data;
    db->data = (student_t *) malloc(db->psize);

    for (i = 0; i < db->lsize; ++i){
        db->data[i] = old[i];
    }

    db->data[db->lsize] = s;
    db->lsize += 1;
    free(old);
}


void db_delete(database_t *db, student_t *s){
    int i;

    if (db->lsize == db->psize){
        db->psize /= 2;
    }

    student_t *old = db->data;
    db->data = (student_t *) malloc(db->psize);

    int part = (s - old)/sizeof(student_t);

    for (i = 0; i < part; ++i){
        db->data[i] = old[i];
    }

    for (i = part+1; i < db->lsize; ++i){
        db->data[i-1] = old[i];
    }

    db->lsize -= 1;
    free(old);
}


void db_save(database_t *db, const char *path){

}

void db_load(database_t *db, const char *path){
    FILE *fo = fopen(path, "r");
    student_t *s = malloc(sizeof(student_t));
    char query[256];

    printf("Loading Database\n");

    //verify if file contains no student
    if (fread(s, sizeof(student_t), 1, fo) == EOF){
        return;
    }

    //put first student in db
    db->data = malloc(sizeof(student_t));
    db->data[0] = *s;
    db->lsize = 1;
    db->psize = sizeof(student_t);

    while(fread(s, sizeof(student_t), 1, fo) != EOF){
        db_add(db, *s);
        student_to_str(query, s);
        printf("%s\n", query);
    }

    printf("Database Loaded");


}


void db_init(database_t *db){

}