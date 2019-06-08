#include "parse.h"

#define MAXLINE 512

char * readLine(){
    char *line = new char[MAXLINE];
    cin.getline(line, MAXLINE);
    return line;
}
//function to remove the starting and ending whitespace from a string
char * trim(char *command){
	int i,j;
	for(i=0;command[i] == ' ';i++);
	for(j=i;command[j] != '\0';j++){
		command[j-i] = command[j];
	}
	command[j-i] = '\0';
	for(j = strlen(command) -1;j >= 0;j--){
		if(command[j] == ' '){
			command[j] = '\0';
		}
		else{
			break;
		}
	}
	return command;
}

/*function to split a command based on the delimiter*/
char ** splitString(char *command,char *delim,int *args_c){
	int ct = 0,i;
	bool inQuotes = false;
	char **args = new char*[MAXLINE/2+1];
	args[0] = new char[MAXLINE];
	*args_c = 0;
	/*go through each and every character in the command*/
	for(i=0;command[i]!='\0';i++){
		/*if the command[i] == delimiter */
		if(command[i] == '"'){
			inQuotes = !inQuotes;
		}
		if(!inQuotes && command[i] == delim[0]){
			bool isDelim = true;
			for(int k=0;k<strlen(delim);k++){
				if(command[i + k] != delim[k]){
					isDelim = false;
					break;
				}
			}
			if(isDelim){
				args[*args_c][ct] = '\0';
				/*reset the ct to start storing the 
				new string from index 0*/
				ct = 0;
				/*go to the next string*/
				*args_c += 1;
				/*allocate memory to the next string*/
				args[*args_c] = new char[MAXLINE];
				i += strlen(delim) - 1;
			}
		}
		else{
			/*store the character into the respective substring */
			args[*args_c][ct] = command[i];
			/*go to the next position*/
			ct++;
		}
	}
	args[*args_c][ct] = '\0';
	*args_c += 1;
	return args;
}

