#include "db.h"
#include "student.h"

#include <stdlib.h>

void db_add(database_t *db, student_t s){
    int i;
    if (db->lsize == db->psize/sizeof(student_t)) {
        db->psize *= 2;
        student_t *old = db->data;
        db->data = (student_t *) malloc(db->psize);
        for (i = 0; i < db->lsize; ++i) {
            db->data[i] = old[i];
        }
        db->data[db->lsize] = s;
        free(old);
    }

    else{
        db->data[db->lsize] = s;
    }
    db->lsize += 1;
}


void db_delete(database_t *db, student_t *s){
    int i;

    if (db->lsize == db->psize/(sizeof(student_t)*4)){
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
    FILE *fo = fopen(path, "w");
    student_t *s = malloc(sizeof(student_t));
    char query[256];
    int i;

    printf("saving database\n");

    fwrite(db->data, sizeof(student_t), db->lsize, fo);

    printf("databse saved!\n");

    fclose(fo);

}


void db_load(database_t *db, const char *path){
    FILE *fo = fopen(path, "rb");
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

    while(fread(s, sizeof(student_t), 1, fo) == 1){
        db_add(db, *s);
    }

    printf("Database Loaded\n");
    fclose(fo);
}


void db_init(database_t *db){
    db->lsize = 0;
    db->psize = 0;
    //db->data = {};
}


void db_select(database_t *db, char *field, char *value){
    int i;
    student_t *s;
    size_t field_offset = 0;

    for (i = 0; i < db->lsize; ++i){
        *s = db->data[i];
        if (strcmp(field, "ID") == 0) {
            if ((char)s->id == *value){
                printf("%c\n", (char)s->id);
            }
        }
        else if (strcmp(field, "fname")==0) { ;}
        else if (strcmp(field, "lname")==0) { ;}
        else if (strcmp(field, "section")==0) { ;}
        else if (strcmp(field, "birthdate")==0) { ;}
    }
}
