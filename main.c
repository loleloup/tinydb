#define __USE_XOPEN
#define _GNU_SOURCE

#include <stdio.h>

int read_file(FILE* file){
    char* line;
    while ((line = std::gets(file)) != EOF){
        std::printf(line)
    }
    return 0;
}


int main(int argc, char const *argv[]) {
    std::printf("Good luck in this projet!\n");
    read_file(std::fopen("db.h", "r"))
    return 0;
}


