#ifndef _DB_H
#define _DB_H

#include "student.h"

/**
 * Database structure type.
 */
typedef struct {
    student_t *data; /** The list of students **/
    size_t lsize;    /** The logical size of the list **/
    size_t psize;    /** The physical size of the list **/
} database_t;

/** 
 *  Add a student to the database.
 * TODO: implement this function.
 **/
void db_add(database_t *db, student_t s);

/**
 * Delete a student from the database.
 * TODO: implement this function.
 **/
void db_delete(database_t *db, student_t *s);

/**
 * Save the content of a database_t to the specified file.
 * TODO: implement this function
 **/
void db_save(database_t *db, const char *path);

/**
 * Load the content of a database of students from a file.
 * TODO: implement this function.
 **/
void db_load(database_t *db, const char *path);

/**
 * Initialise a database_t structure.
 * Typical use:
 * ```
 * database_t db;
 * db_init(&db);
 * ```
 * TODO: implement this function.
 **/
void db_init(database_t *db);

void db_find(database_t *db, char *field, char *value);

#endif