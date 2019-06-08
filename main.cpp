#include "execute.h"
#include "parse.h"
#include "connector.h"


int main(int argc, char const *argv[])
{
	char *line = NULL;
	while(true){
		cout << "$";
		line = readLine();
		if(strcmp(line,"") == 0){
			continue;
		}
		parseConnectorsExecute(line);
	}
	return 0;
}
