#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <poll.h>
#include <signal.h>

// Stuff
/*
Basic Algorithm
0) Start Prompt
1) Tokenify the Input Lines
	a) Ignore comments (#). Whitespace doesn't matter
	b) Semi-colon is the delimeter.
	c) Be able to handle any options on commands (-l, -s, etc).
2) Fork on new commands
3)
4) Print to command line:
	a) Linked List
	b) CPU Resource Statistics (formatted)
		i) amount of time program spent executing in user space
		ii) amount of time spent executing in kernel space.
*/
char** tokenify(const char *s) {
    // input a constant string --> return an array of token strings (that were separated by whitespace)
	int count = 0;
	char *copy = strdup(s);
	char *token;
	char **words=malloc(sizeof(char*)*(strlen(s)));
	token = strtok(copy ,";");
	while (token!=NULL){
		//Error case is null...so no redundancy needed 
		if (strstr(token, "#")==NULL){
		//if the line contains a #, ignore the rest of the tokens.
			words[count]=strdup(token);
			token = strtok(NULL, ";");
			count++;	
		}
		else{
			if (token[0]=='#'){
				break;
			}
			//slice up to #
			char *token_hash=strtok(token, "#");
			words[count]=strdup(token_hash);
			count++;
			break;
		}
	}
	words[count]=NULL;
	free(copy);
	for (int i=0; i<count; i++){
		//printf("Token %i: %s\n", i, words[i]);
		//fflush(stdout);
	}
	return words;
	
	
}
// parses a single command, returns a pointer to each element
// first element will be the file path
// all following elements will be flags
char** parse_command(const char *s) {
    char *str = strdup(s);
    int index = 0;

    int numWhiteSpaces = 0;
    for (int i = 0; i < strlen(s); i++){
    	if (isspace(s[i])){
    	    numWhiteSpaces++;
    	}
    }

    // allocate array of pointers
    char **returnArray = (char**) malloc(sizeof(char*) * numWhiteSpaces+5);

    // "Go red sox!"
    char *token = malloc(sizeof(char));
    //char *
    token = strtok(str, " \n\t"); // pointer to first word...


    while (token != NULL){
    	token = strdup(token);
    	returnArray[index] = token;
    	index++;
    	token = strtok(NULL, " \n\t");
    }
    returnArray[index] = NULL;

    return returnArray; // pointer to rv
}

// int validate_command(char **s){
	
// 	int i = 0;
// 	printf("a\n");
// 	while (*s[i] != NULL){
// 		printf("%s\n", *s[0]);

// 		char* copy = strdup(*s[i]);
// 		// file path
// 		if (i == 0) {
// 			if (copy[0] != '/'){
// 				printf("String beings with /");
// 				return 0; // false, file path must start with slash
// 			}
// 			if (copy[strlen(copy)-1] = "/"){
// 				return 0; // false. a tailing '/' -> no program given
// 			} 
// 		}

// 		// flags
// 		else {
// 			if (copy[0] != '-'){
// 				printf("Flag didn't have a dash");
// 				return 0;
// 			}

// 			// 

// 			// if (strlen(copy) > 2){
// 			// 	return 0; 
// 			// }
// 		}
// 		i++;
// 	}

// 	char** copy = strdup(s);
// 	printf("The copy: %s\n", copy);
// 	printf("Copy[0]: %c\n", copy[0]);

	

// }

void print_tokens(char *tokens[]) {
    int i = 0;
    while (tokens[i] != NULL) {
        printf("Token %d: %s\n", i+1, tokens[i]);
        i++;
    }
}
int overhead(const char *line, int sequential){
		if (strcmp(line, "exit")==0){
			return 1;
		}
		if (strcmp(line, "mode")==0){
			if (sequential==1){
				printf("Mode is Sequential\n");
				return 0;
			}
			else {
				printf("Mode is Parallel\n");
				return 0;
			}
		}
		
		if (strcmp(line, "sequential")==0 || strcmp(line, "mode s")==0){
			sequential = 1;
			return 2;
		}
		if (strcmp(line, "parallel")==0 || strcmp(line, "mode p")==0){
			sequential = 0;
			return 3;;
		}
		return 0;
}

int main(int argc, char **argv) {

	//const char *tmp1 = "/bin/ls -p -f ; /bin/pwd -w";
	//char **tokens = tokenify(tmp1);
    //mchar **results = parse_command(tokens[0]);
    //print_tokens(results);
    //validate_command(results);

	//get Input
	int sequential = 1;
	int count = 0;
	printf("Prompt: ");
	fflush(stdout);  
	char line[1024];
	while (fgets(line, 1024, stdin) != NULL) {
		for (int i =0; i<1024; i++){
			if (line[i]=='\n'){
				line[i]='\0';
			}
		}
	
		//tokenify the line. Returns separate commands.
		char ** tokens = tokenify(line);
		int exit_terminal = 0;
		int mode_switch = 0;
		if (sequential) {
			for (int i=0; tokens[i]!=NULL; i++){
				char ** results = parse_command(tokens[i]);
				int overhead_command = overhead(tokens[i], sequential);
				//if (overhead_command==0) do nothing. print mode.
				if (overhead_command==1){
					//exit command issued
					exit_terminal = 1;
					continue;
				}
				if (overhead_command==2){
					//switch to sequential mode
					mode_switch = 1;
					continue;
				}
				if (overhead_command==3){
					//switch to parallel
					mode_switch = 2;
					continue;
				}


				pid_t pid = fork();
				if (pid==0){

					if (execv(results[0],results)<0){
						printf("Process entered is wrong.\n");
						exit(0);
						printf("Exited, something messed up."); //should never print
					}
				}
				else {
					wait(&pid);
				}
			}

			if (exit_terminal==1){
				return 0;
			}
			if (mode_switch==1){
				sequential = 1; //switch to sequential
			}
			if (mode_switch==2){
				sequential = 0; //switch to parallel
			}
		}
		else {
			for (int i=0; tokens[i]!=NULL; i++){
				char ** results = parse_command(tokens[i]);
				pid_t pid = fork();
				if (pid==0){
					if (execv(results[0],results)<0){
						printf("Process Failed\n");
						exit(0);
					}

				}
				
			}
		}
		printf("Prompt: ");
		

		count ++;
	}


	return 0;
}

