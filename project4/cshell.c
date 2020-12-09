#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>



int exiting = 0;
pid_t pid = -1;
int killing = 0;
char * lastPath;
int fd;


void ctrlCSignal(int signum){
	//printf("makes it inside ;)");
    
	if (pid != 0){
   		kill(pid, 0);
		killing = 1;
	}
};	

int redirection(char *toCopy, char *newFile, int mode){	//copies the file of toCopy to newFile

	fd = open(newFile, 777);
	if (fd < 0){
		creat(newFile, 777);
	}

	if (mode == 1){ //for "">"
		dup2(fd, 1);
		dup2(fd, 2);
		creat(newFile, 777);
		
	}
	else if (mode == 2){ //for ">>"
		dup2(fd, 1);
		dup2(fd, 2);

	}
	



	return 0;
}


int main(int argc, char *argv[]){

	signal(SIGINT, ctrlCSignal); //used to signal ctrl-C

	//holds the path to Home
	char homePath[1000];
	strcpy(homePath, getenv("HOME"));
	
	//holds the last path visited
	char temppath[1000];
	getcwd(temppath, 1000);
	lastPath = (char *)malloc((strlen(temppath)+1) *sizeof(char));
	strcpy(lastPath, temppath);

	
	
	///context switch here
	while (exiting == 0) {
		//gets the current working directory to set up prompt
   		char currDirr[1000];
    	getcwd(currDirr, 1000);
    	printf("%s $ ", currDirr);

		//gets an input command from stdin
		char buf[500];
        fgets(buf, 500, stdin);

		
        char * cmd = strtok(buf, ";\n");
		printf("cmd = %s\n", cmd);
		
		
		while (cmd != NULL){
			
			//printf("%s\n", cmd);

			////for exit
			int index = 0;
			for (int i = 0; i < strlen(cmd); i++){
				index = i;
				if (cmd[i] != ' '){
					break;
				}
			}
			if ((cmd[index] == 'e' ) && ((strstr(cmd, "exit") != NULL && (strlen(cmd) < index + 5) )|| strstr(cmd, "exit ") != NULL)){
				printf("exiting\n");
				return 0;
			}
            ////for exit


			//creating the pointers that will hold the arguments and executed command
			char * args[128]; //holds each argument separated by spaces
			char * currcmdBuf = (char *)malloc((strlen(cmd)+1)*sizeof(char));
			strcpy(currcmdBuf, cmd);	//holds the command to be run
			printf("currcmd = %s99\n", currcmdBuf);
			char * currcmd = strtok(currcmdBuf, " \n");	//get the command here 
			//creating the pointers that will hold the arguments and executed command

			//for change directory
			if (cmd[index] == 'c' && ((strstr(cmd, "cd") != NULL && (strlen(cmd) < index + 3)) || strstr(cmd, "cd ") != NULL)){
				currcmd = strtok(NULL, " \n");
			
				char *lastPathTemp = (char *)malloc((strlen(currDirr) + 1)*sizeof(char));
				strcpy(lastPathTemp, currDirr);
				int changed;

				if (currcmd == NULL || strcmp(currcmd, "~") == 0){ //changing to home directory
					changed = chdir(homePath);
					if (changed == -1){
						printf("Path could not be found\n");
					}
					else{
						memset(lastPath, '\0', strlen(lastPath) *sizeof(char));
						strcpy(lastPath, lastPathTemp);
					}
					
				}
				else if (currcmd[0] == '~'){ //changing to new directory from the home directory
					currcmd++;
					char moveHome[1000];
					strcat(moveHome, homePath);
					strcat(moveHome, currcmd);
					changed = chdir(moveHome);
					if (changed == -1){
						printf("Path could not be found\n");
					}
					else{
						memset(lastPath, '\0', strlen(lastPath) *sizeof(char));
						strcpy(lastPath, lastPathTemp);
					}
				}
				else if (strcmp(currcmd, "-") == 0){	//changing to last visited directory
					changed = chdir(lastPath);

					memset(lastPath, '\0', strlen(lastPath) *sizeof(char));
					strcpy(lastPath, lastPathTemp);	
				}
				else { //changing directory by pathname
					changed = chdir(currcmd);
					if (changed == -1){
						printf("incorrect command or pathway\n");
					}
					else{
						memset(lastPath, '\0', strlen(lastPath) *sizeof(char));
						strcpy(lastPath, lastPathTemp);
					}
				}

				getcwd(currDirr, 1000);
				cmd = strtok(NULL, ";\n");
				printf("cmd = %s\n", cmd);
				continue;
			}
			//for change directory


			//creating the pointers that will hold the arguments and executed command
			char * cmdToExec = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
			strcpy(cmdToExec, currcmd);	//store the command in a string
			
			
			int count = 0;
			while (currcmd != NULL) { //populates the args array
				printf("%s ", currcmd);
				printf("%lu\n", strlen(currcmd));
				args[count] = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
				strcpy(args[count], currcmd);
				args[count+1] = NULL;
				currcmd = strtok(NULL, " \n");
				
				count++;
			}
			//creating the pointers that will hold the arguments and executed command




			printf("cmd and args:\n");
			printf("%s\n", cmdToExec);
			for (int i = 0; i < count; i++){
				printf("%s, ", args[i]);
			}
			printf("\n%d\n", count);

			
			//look for redirection


			//to run a command
			pid = fork();
	
			if (pid == 0) {
				printf("here\n");
				execvp(cmdToExec, args);
				return 0;
			} else {
				wait(NULL);
				pid = 0;
			}
			pid = 0;
		
			//to run a command

			
			//goes to the next command separated by ";"
			cmd = strtok(NULL, ";\n");
			printf("cmd = %s\n", cmd);
		}
	}




	return 0;
}
