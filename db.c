#include "db.h"

#include <stdlib.h>

void db_add(database_t *db, student_t s){
    int i;

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
    student_t *old = db->data;
    db->psize -= sizeof(student_t);
    db->data = (student_t *) malloc(db->psize);

    part = (s - db->data)/sizeof(student_t);

    for (i = 0; i < part; ++i){
        db->data[i] = old[i];
    };

    for (i = part+1; i < db->lsize; ++i){
        db->data[i-1] = old[i];
    };

    db->lsize -= 1;
    free(old);
}


void db_save(database_t *db, const char *path){

}

void db_load(database_t *db, const char *path){

}


void db_init(database_t *db){

}