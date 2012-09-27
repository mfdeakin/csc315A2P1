
CC=gcc
CFLAGS=-g -pg -Wall -std=c99
LIBS=-lGL -lglut -lGLU

prog: main.o matrix.o list.o draw.o
	${CC} ${CFLAGS} ${LIBS} matrix.o main.o list.o draw.o -o prog