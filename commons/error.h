#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

int error(char* msg);
int info(char* msg);
void longToChar(long l, char* str);
bool equal(char* a, char* b);
int parse(char* cmd, char*** words);
long charToLong(char* a);
int charToInt(char* a);
long readLine(FILE* f, char* buf);

#endif
