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
			token = strtok(NULL, " \n\t");
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
	return words;
	
	
}



int main(int argc, char **argv) {
	//get Input
	int sequential = 1
	int count = 0;
	printf("Prompt: ");
	fflush(stdout);  
	char line[1024];
	while (fgets(line, 1024, stdin) != NULL) {
		if (count!=0){
			printf("Prompt: ");
		}
		if (strcmp(line, "exit")==0){
			printf("Exiting \n");
			wait(); //wait for all other processes to finish.
			return 0;
		}
		if (strcmp(line, "mode")==0){
			if (sequential==1){
				printf("Mode is Sequential\n");
			}
			else {
				printf("Mode is Parallel\n");
			}
		}
		if (strcmp(line, "parallel")==0 || strcmp(line, "mode p")==0){
			sequential = 0;
		}
		if (strcmp(line, "sequential")==0 || strcmp(line, "mode s")==0){
			sequential = 1;
		}
		
	
		//tokenify the line. Returns separate commands.
		char ** tokens = tokenify(line);
		
	
		if (sequential) {
			// sequential stuff
		}
		else () {
			// parallel stuff
		}


	count ++;
	}


	return 0;
}

