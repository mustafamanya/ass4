#include "../header/parse.h"
#include "../header/execute.h"
#include <sys/stat.h>

int pidArray[MAX_COMMANDS];

int dupPipe(int pip[2], int end, int destfd){
    if(dup2(pip[end],destfd)<0){
        return -1;
    }
    if(close(pip[end])<0){
        return -1;
    };
    return destfd;
}

int redirect(char *filename, int flags, int destfd){
    if (flags==0){
        int fd = open(filename, O_RDONLY);
        if (fd<0){
            return -1;
        } 
        else {
            if(dup2(fd, destfd) < 0){
                return -1;
            };
        }
    } 
    else {
        remove(filename);
        if(access(filename, W_OK)==0){
            return -1;
        } 
        else {
            int fd2 = open(filename, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU );
            if (fd2<0){
                return -1;
            }
            if(dup2(fd2, destfd) < 0){
                return -1;
            }
        }
    }
    return destfd;
}
//function that executes all the commands
//creates pipeline when required
bool processCommands(command comLine[], int nCommands){
    pid_t pid;
    int in = 0;
    int fd[2];
    int status;
    int redir = 0;
    int pidCount = 0;
    for(int commandIndex = 0; commandIndex < (nCommands -1); commandIndex++){
        if(pipe(fd)){
            perror("pipe:");
            return false;
            exit(EXIT_FAILURE);
        };
        pid = fork();
        if(pid == -1){
            perror("fork:");
            return false;
            exit(EXIT_FAILURE);
        }
        pidArray[pidCount] = pid;
        pidCount++;
        if(pid == 0){
            if(commandIndex == 0 && comLine[0].infile != NULL){
                redirect(comLine[0].infile,0,READ_END);
                redir = 1;
            } else {
                if(close(fd[READ_END])<0){
                    perror("close fd[READ_END]");
                    return false;
                }
            }
            if (in != 0){
                dup2(in, 0);
                if(close(in)<0){
                    perror("close in, child proc");
                    return false;
                }
            }
            if (fd[WRITE_END] != 1){
                dupPipe(fd, WRITE_END, 1);
            }
            if(execvp (comLine[commandIndex].argv[0],
                       comLine[commandIndex].argv )<0){
                perror(comLine[commandIndex].argv[0]);
            	return false;
                exit(EXIT_FAILURE);
            }

        } else {
            if (commandIndex < (nCommands - 1) && in != 0 ){
                if(close(in)<0){
                    perror("close in, parent proc");
                    return false;
                }
            }
            in = fd[READ_END];
            if(close(fd[WRITE_END])<0){
                perror("close fd[WRITE_END]");
                return false;
            }
        }
    }
    pid = fork();
    if(pid == -1){
        perror("fork:");
        return false;
        exit(EXIT_FAILURE);
    }
    pidArray[pidCount] = pid;
    if(pid == 0){
        if(nCommands == 1 &&  comLine[0].infile != NULL){
            redirect(comLine[0].infile,0, READ_END);
            redir = 1;
        }
        if(comLine[nCommands-1].outfile != NULL){
            redirect(comLine[nCommands-1].outfile,1,WRITE_END);
        }
        if(nCommands != 1 ){
            dupPipe(fd, READ_END, 0);
        }
        if(execvp(comLine[nCommands - 1].argv[0],
                  comLine[nCommands - 1].argv)<0){
            perror(comLine[nCommands -1].argv[0]);
        	return false;
            exit(EXIT_FAILURE);
        };
    }
    for(int currentPid = 0; currentPid<MAX_COMMANDS; currentPid++){
        if (pidArray[currentPid]!=0){
            waitpid(pidArray[currentPid], &status,WUNTRACED);
        }
    }
    if(redir != 1 && nCommands > 2 ){
        if(close(fd[0])<0){
            perror("close fd[0]");
            return false;
        }
    }
    return true;
}

bool runCommand(char *cmdStr){
    int numCommands = 0;
    char **commandsStr = splitString(cmdStr,"|",&numCommands);
    command commands[numCommands];
    for(int i=0;i<numCommands;i++){
    	commandsStr[i] = trim(commandsStr[i]);
    	int numTokens = 0;
    	char **tokens = splitString(commandsStr[i]," ",&numTokens);
    	commands[i].argc = numTokens;
    	commands[i].argv = new char *[numTokens+1];
    	for(int ii=0;ii<numTokens;ii++){
    		tokens[ii] = trim(tokens[ii]);
    		commands[i].argv[ii] = new char[strlen(tokens[ii])+1];
    		strcpy(commands[i].argv[ii],tokens[ii]);
    	}
    	commands[i].argc ++;
    	commands[i].argv[numTokens] = NULL;
    }
    for(int i=0;i<numCommands;i++){
        int cmd = commands[i].argc;
        for(int ii=0;ii<commands[i].argc;ii++){
            if(commands[i].argv[ii] != NULL && (strcmp(commands[i].argv[ii],">") == 0 || strcmp(commands[i].argv[ii],">>") == 0)){
                commands[i].outfile = (char *)malloc(strlen(commands[i].argv[ii+1])+1);
                strcpy(commands[i].outfile,commands[i].argv[ii+1]);
                cmd = (cmd > ii) ? ii : cmd;
                commands[i].argv[ii] = NULL;
                commands[i].argv[ii+1] = NULL;
                ii++;
            }
            if(commands[i].argv[ii] != NULL && (strcmp(commands[i].argv[ii],"<") == 0|| strcmp(commands[i].argv[ii],"<<") == 0)){
                commands[i].infile = (char *)malloc(strlen(commands[i].argv[ii+1])+1);
                strcpy(commands[i].infile,commands[i].argv[ii+1]);
                cmd = (cmd > ii) ? ii : cmd;
                commands[i].argv[ii] = NULL;
                commands[i].argv[ii+1] = NULL;
                ii++;
            }
        }
        commands[i].argc = cmd;
    }
    //check for cd command
    if(strcmp(commands[0].argv[0],"cd") == 0){
        if(chdir(commands[0].argv[1])!=0){
            fprintf(stderr, "cd: %s : ", commands[0].argv[1]);
            perror("");
            return false;
        }
    }
    //check for exit
    //sentinal value for the loop
    else if(strcmp(commands[0].argv[0],"exit") == 0){
        exit(0);
        return true;
    }
    //process any other command
    else{
        return processCommands(commands,numCommands);
    }
}

/*function to execute a parsed Comamnd*/
bool executeCommand(char *command){
	char **args;
	int args_c,status;
	if(strcmp(command,"") == 0 || strcmp(command,"true") == 0){
		return true;
	}
	else if(strcmp(command,"false") == 0){
		return false;
	}
	/*if command is not empty string*/
	if(strcmp(command,"")){
		if(command[0] == '[' && command[strlen(command)-1] == ']'){
			int num;
			char **tok = splitString(command,"]",&num);
			if(num != 2){
				return false;
			}
			char **tok2 = splitString(tok[0],"[",&num);
			if(num != 2){
				return false;
			}
			char cmd[512];
			strcpy(cmd,"test ");
			strcat(cmd,tok2[1]);
			return executeCommand(trim(cmd));
		}
		/*Tokenize the command*/
		args = splitString(command," ",&args_c);
		if(strcmp(args[0],"test") == 0){
			if(args_c != 3){
				return false;
			}
			if(strcmp(args[1],"-e") == 0){
				struct stat info;
				if(lstat(args[2],&info) != 0) {
				  if(errno == ENOENT) {
				   	cout << "(False)\n";
				   } else if(errno == EACCES) {
				    cout << "(Permission denied)\n";
				   } else {
				      cout << "(Error: "<<errno << ")\n";
				   }
				  return true;
				}
				cout << "(True)\n";
			}
			else if(strcmp(args[1],"-f") == 0){
				struct stat info;
				if(lstat(args[2],&info) != 0) {
				  if(errno == ENOENT) {
				   	cout << "(False)\n";
				   } else if(errno == EACCES) {
				    cout << "(Permission denied)\n";
				   } else {
				      cout << "(Error: "<<errno << ")\n";
				   }
				  return true;
				}
				if(S_ISREG(info.st_mode)) {
					cout << "(True)\n";
				}
				else{
					cout << "(False)\n";
				}
			}
			else if(strcmp(args[1],"-d") == 0){
				struct stat info;
				if(lstat(args[2],&info) != 0) {
				  if(errno == ENOENT) {
				   	cout << "(False)\n";
				   } else if(errno == EACCES) {
				    cout << "(Permission denied)\n";
				   } else {
				      cout << "(Error: "<<errno << ")\n";
				   }
				  return true;
				}
				if(S_ISDIR(info.st_mode)) {
					cout << "(True)\n";
				}
				else{
					cout << "(False)\n";
				}
			}
			return false;
		}
		return runCommand(command);
		// /*fork a new process to execute the command*/
		// if(fork() == 0){
		// 	execvp(args[0],args);
		// 	return false;
		// }
		// wait(&status);
	}
	return true;
}

