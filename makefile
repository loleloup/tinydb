FLAGS=-pthread -std=c11 -Wall -Werror -Wpedantic


main: main.c db.h student.h parsing.h
	gcc -o tinydb main.c parsing.o student.o ${FLAGS}

run:
	make main && ./main

tests: tests/run_tests.py
	./tests/run_tests.py

clean:
	rm logs/*
