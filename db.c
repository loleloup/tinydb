#include "db.h"
#include "student.h"

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>

void db_add(database_t *db, student_t s){
    int i;
    char buffer[64];

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
    student_to_str((char *)buffer, &s);
    //printf("added %s\n", buffer);

}


void db_delete(database_t *db, student_t *s){
    int i = 0;
    if (db->lsize == db->psize/(sizeof(student_t)*4)){
        db->psize /= 2;
        student_t *old = db->data;
        db->data = (student_t *) malloc(db->psize);
        while (!student_equals(&old[i] ,s) && i < db->lsize){
            db->data[i] = old[i];
            i++;
        }
        db->lsize -= 1;
        while (i < db->lsize){
            db->data[i] = old[i+1];
            i++;
        }

        free(old);
    }

    else{
        while (!student_equals(&db->data[i], s) && i < db->lsize){
            i++;
        }
        db->lsize -= 1;
        while (i < db->lsize){
            db->data[i] = db->data[i+1];
            i++;
        }
    }


}


void db_save(database_t *db, const char *path){
    FILE *fo = fopen(path, "w");

    printf("saving database\n");

    fwrite(db->data, sizeof(student_t), db->lsize, fo);

    printf("database saved!\n");

    fclose(fo);

}


void db_load(database_t *db, const char *path){
    FILE *fo = fopen(path, "rb");
    student_t *s = malloc(sizeof(student_t));

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
    db->psize = sizeof(student_t)*10;
    db->data = malloc(sizeof(student_t)*10);
}


void db_update(database_t *db, student_t *old, student_t *updated){
    int i;

    while (!student_equals(&db->data[i], old) && i < db->lsize){
        i++;
    }
    db->data[i] = *updated;
}

void db_empty(database_t *db){
    free(db->data);
    db_init(db);
}
