#pragma once

#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

char ** splitString(char *command,char *delim,int *args_c);
char *readLine();
char * trim(char *command);