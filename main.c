#define __USE_XOPEN
#define _GNU_SOURCE

#include "student.c"
#include <stdio.h>
#include <pthread.h>

int read_file(FILE* file){
    char* line;
    while ((line = fgets("\n", 100, file)) != NULL){
        printf("%s", line);
    };
    return 0;
}


int main(int argc, char const **argv) {
    char query[256];
    student_t *s;
    //int running = 1;
    FILE* fo = fopen("/home/me/Téléchargements/students.bin(1)", "r");
    printf("%zu \n", sizeof(student_t));
    printf("%zu \n", sizeof(*s));
    fread(s, sizeof(student_t), 1, fo);

    printf("Good luck in this projet!\n");

    //fgets(query, 100, stdin)
    while (fread(s, sizeof(student_t), 1, fo) != EOF){
        student_to_str(query, s);
        printf("%s", query);
    }

    return 0;
}


