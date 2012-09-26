
CC=gcc
CFLAGS=-g -Wall -DMTXDEBUG -std=c99
LIBS=-lGL -lglut -lGLU

prog: main.o matrix.o list.o draw.o
	${CC} ${LIBS} matrix.o main.o list.o draw.o -o prog