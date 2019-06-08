#include "../header/connector.h"
#include "../header/parse.h"
#include "../header/execute.h"
#include <stack>
 
int precedence(string op){ 
    if(op == "&&" || op == "||") {
    	return 1;  
    }
    return 0; 
}

string applyOp(string a, string b, string op){ 
	char aa[512],bb[512];
	strcpy(aa,a.c_str());
	strcpy(bb,b.c_str());
    if(op == "&&"){
    	if(executeCommand(trim(aa)) && executeCommand(trim(bb))){
    		return "true";
    	}
    } 
    else if(op == "||"){
    	if(executeCommand(trim(aa)) || executeCommand(trim(bb))){
    		return "true";
    	}
    }
    return "false";
} 

bool evaluate(string tokens){ 
    int i; 
    stack <string> values;  
    stack <string> ops; 
    string currentCommand = "";
    for(i = 0; i < tokens.length(); i++){ 
        if(tokens[i] == '('){ 
        	ops.push("(");
        } 
        else if(tokens[i] == ')') { 
        	values.push(currentCommand);
        	currentCommand = string("");
            while(!ops.empty() && ops.top() != "(") { 
                string val2 = values.top(); 
                values.pop(); 
                  
                string val1 = values.top(); 
                values.pop(); 
                  
                string op = ops.top(); 
                ops.pop(); 
                values.push(applyOp(val1, val2, op));  
            }
            ops.pop(); 
        } 
        else if((tokens[i] == '&' && i + 1 < tokens.length() && tokens[i+1] == '&')||(tokens[i] == '|'  && i + 1 < tokens.length() && tokens[i+1] == '|'))
        {  
            values.push(currentCommand);
            currentCommand = string("");
            if(tokens[i] == '&' && tokens[i+1] == '&'){
            	ops.push("&&");
            	i++;
            }
            else if(tokens[i] == '|' && tokens[i+1] == '|'){
            	ops.push("||");
            	i++;
            }
        } 
        else{
        	currentCommand.push_back(tokens[i]);
        }
    } 
    values.push(currentCommand);
    stack<string> revVal;
    while(!values.empty()){
    	revVal.push(values.top());
    	values.pop();
    }
    stack<string> revOps;
    while(!ops.empty()){
    	revOps.push(ops.top());
    	ops.pop();
    }
    while(!revOps.empty()){ 
        string val1 = revVal.top(); 
        revVal.pop(); 
        string val2 = revVal.top(); 
        revVal.pop(); 
        string op = revOps.top(); 
        revOps.pop(); 
                  
        revVal.push(applyOp(val1, val2, op));  
    } 
    string res = revVal.top();
    if(res != "true" || res != "false"){
    	res = applyOp(revVal.top(),"true","||");
    }
    return res == "true";
} 

void parseConnectorsExecute(char *command){
	int numCmds = 0;
	char **commands = splitString(command,";",&numCmds);
	for(int i=0;i<numCmds;i++){
		commands[i] = trim(commands[i]);
		evaluate(commands[i]);
	}
 }
