#ifndef SHELL_H
#define SHELL_H
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <string.h>

char* checkSpaces( char* cd);
char* checkCMD(char* cmd);
void numCharactersinString(char* filepath, int numArray[2]);
//void addToBuffer(char* buf, char* filepath, int count);
void redirectionOrPiping(int redirCount, char** args, int* redirectIndexes, int redirSupported, int argcount, int input, int output, char* directory, char** envp);
void newMethod(int redirCount, char** args, int* redirectIndexes, int *redirSupported, int argcount, int input, int output, char* directory, char** envp, char** execdirectories, int numargs);
int Atoui(char *inputWord);
char* inttocharacter(int num, char numberstring[]);
int stringlength(char* string);

#endif
