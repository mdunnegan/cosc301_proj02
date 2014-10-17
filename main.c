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
#include "list.h"

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

void print_tokens(char *tokens[]) {
    int i = 0;
    while (tokens[i] != NULL) {
        printf("Token %d: %s\n", i+1, tokens[i]);
        i++;
    }
}

int overhead(char **line, int sequential){
		int count =0;
		for (int i = 0; line[i]!=NULL; i++){
				//count number of terms in a command.
				count++;
		}
		if (count==0){
				//if there are no terms, do nothing.
				return 0;
		}
		else{
				if (count==1){
						//if 1 term, there is  a limited number of options. 
						if (strcmp(line[0], "exit")==0){
								return 1;
						}
						if (strcmp(line[0], "mode")==0){
								if (sequential==1){
									printf("Mode is Sequential\n");
									return 4;
								}
								else {
									printf("Mode is Parallel\n");
									return 4;
								}
						}
						if (strcmp(line[0],"sequential")==0){
								return 2;
						}
						if (strcmp(line[0],"parallel")==0){
								return 3;
						}
				}
				if (count==2){
					//likewise if there is only 2 terms then there is a limited number of options.
						if ( strcmp(line[0],"mode")==0 && strcmp(line[1], "s")==0){
								return 2;
						}
						if (strcmp(line[0], "mode")==0 && strcmp(line[1], "p")==0){
								return 3;;
						}
				}
			//if (count>2) then this is not an overhead command.
		}
		return 0;
}

int main(int argc, char **argv) {

	  //get Input
	  int sequential = 1;
	  int count = 0;
	  printf("Prompt: ");
	  fflush(stdout);
	  char line[1024];

	  for(;fgets(line, 1024, stdin) != NULL; printf("Prompt: ")) {
		    for (int i =0; i<1024; i++){
			      if (line[i]=='\n'){
			        line[i]='\0';
			      }
		    }

		    //tokenify the line. Returns separate commands.
		    char ** tokens = tokenify(line);
		    int exit_terminal = 0;
		    int mode_switch = 0;
		    pid_t pid;

		    for (int i=0; tokens[i]!=NULL; i++){
	          char ** results = parse_command(tokens[i]);
	          int overhead_command = overhead(results, sequential);
	          //if (overhead_command==0) do nothing.
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
	          if (overhead_command==4){
	              //already printed what mode we are in
	              continue;
	          }

	          pid = fork();

	          if (pid==0){
								FILE *fp;
								fp = fopen("shell-config", "r");
								char holder[20];
								// initialize linked list to store paths
								struct node **head = NULL;
								while (fscanf(fp, "%s/n", holder) != EOF) {
								    list_insert(holder, &head);
								}

								list_print(head);

								struct node *iterator = *head;
								struct stat statresult;
								int rv = stat(results[0], &statresult);
								if (rv < 0){
									  // stat failed
										printf("%s", (*head) -> name);
									  while (iterator != NULL){
									  		printf("a");

									  	  char new_string[40];
									  	  strcat(new_string, iterator->name);
									  		strcat(new_string, "/");

									  		strcat(new_string, results[0]);
									  		rv = stat(iterator->name, &statresult);
									  	  if (rv < 0){
									  	  	  iterator = iterator -> next;
									  	  	  continue;
									  	  }
									  	  results[0] = new_string;
									  	  iterator = iterator -> next;
									  }

								}

								printf("1\n");
	              if (execv(results[0],results)<0){
	          	      printf("2\n");
	                  printf("Process entered is wrong.\n");
	                  exit(0);
	                  printf("Exited, something messed up."); //should never print
	              }
	          }
	          if (sequential) {
	              wait(&pid);
	          }
		    }

		    if (sequential == 0){
		        wait(&pid);
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

		    count ++;
	  }

	  return 0;
}

