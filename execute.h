#pragma once

#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <fcntl.h>
#include <ctype.h>

using namespace std;

#define WRITE_END 1
#define READ_END 0
#define MAX_LENGTH 1024
#define MAX_COMMANDS 8

typedef struct command_t
{
    char **argv;
    int argc;
    char *infile;
    char *outfile;
    bool append;
} command;

int dupPipe(int pip[2], int end, int destfd,bool append);
int redirect(char *filename, int flags, int destfd,bool append);
bool processCommands(command comLine[], int nCommands);
bool runCommand(char *cmdStr);
bool executeCommand(char *command);
