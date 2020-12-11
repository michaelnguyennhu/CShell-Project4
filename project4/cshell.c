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

int redirection(char *newFile, int mode){	//copies the file of toCopy to newFile
	
	
	int flags = O_CREAT|O_WRONLY| (mode == 'w' ? O_TRUNC : O_APPEND);
	fd = open(newFile, flags, 0666);

	if (mode == 1){ //for "">"
		int hi = dup2(fd, 1);
		int hello = dup2(fd, 2);
		creat(newFile, 0666);
		
	}
	else if (mode == 2){ //for ">>"
		dup2(fd, 1);
		dup2(fd, 2);

	}
	



	return 0;
};


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

	while (exiting == 0) {
		//gets the current working directory to set up prompt
   		char currDirr[1000];
    	getcwd(currDirr, 1000);
    	printf("%s $ ", currDirr);

		//gets an input command from stdin
		char buf[5000];
        fgets(buf, 5000, stdin);

		
        char *buftemp = buf;
		char *cmd;
		
		
		while ((cmd = strtok_r(buftemp, ";\n", &buftemp))){
			
			//printf("cmd = %s\n", cmd);

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



			



			//for change directory
			if (cmd[index] == 'c' && ((strstr(cmd, "cd") != NULL && (strlen(cmd) < index + 3)) || strstr(cmd, "cd ") != NULL)){
				//creating the pointers that will hold the arguments and executed command
				char * args[128]; //holds each argument separated by spaces
				char * currcmdBuf = (char *)malloc((strlen(cmd)+1)*sizeof(char));
				strcpy(currcmdBuf, cmd);	//holds the command to be run
				char * currcmd = strtok(currcmdBuf, " \n");	//get the command here 
				//creating the pointers that will hold the arguments and executed command



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
				continue;
			}
			//for change directory
			

			
			//handles redirection with and without piping
			if (strstr(cmd,">") != NULL){
				if (strstr(cmd, ">>") != NULL){ //this means ">>" //use mode 2
					//check for piping and run the command and redirect the output 
					if (strstr(cmd, "|") != NULL){ //here we can use strtok_r
						//printf("piping with no double redirection\n");

						char *forRedir = (char *)malloc((strlen(cmd) + 1) * sizeof(char));
						strcpy(forRedir, cmd);
						char *redir2 = forRedir;
						char *redir = strtok_r(redir2, ">", &redir2);// now you have the first token
						char *token1 = (char *)malloc((strlen(redir) + 1) * sizeof(char));
						strcpy(token1, redir);
						redir = strtok_r(redir2, " >", &redir2);//now you have the second token


						//now you deal with piping in the first token
						char *pipeBuffer = token1;
						char *newCmd;
						


						int pipefd[2];
						pid_t pipepid;
						int newfd = 0;

						while ((newCmd = strtok_r(pipeBuffer, "|", &pipeBuffer))){ //for each pipe encountered till end of the string 
							
							
							char * args[128]; //holds each argument separated by spaces
							char * currcmdBuf = (char *)malloc((strlen(newCmd)+1)*sizeof(char));
							strcpy(currcmdBuf, newCmd);	//holds the command to be run
							char * currcmd = strtok(currcmdBuf, " \n");	//get the command here 
							//creating the pointers that will hold the arguments and executed command

							
							//creating the pointers that will hold the arguments and executed command
							char * cmdToExec = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
							strcpy(cmdToExec, currcmd);	//store the command in a string
							
										
					
							int count = 0;
							while (currcmd != NULL) { //populates the args array
								
								args[count] = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
								strcpy(args[count], currcmd);
								args[count+1] = NULL;
								currcmd = strtok(NULL, " \n");
						
								count++;
							}

							pipe(pipefd);
							pid = fork();

							if (pid == -1) {
								perror("error");
								exit(1);
							} else if (pid == 0) {
								dup2(newfd, 0);
								if (strlen(pipeBuffer) != 0){
									dup2(pipefd[1], 1);
								}
								else {
									redirection(redir, 2);
								}
								close(pipefd[0]);
								execvp(cmdToExec, args);
								exit(1);
							} else {
								wait(NULL);
								if (killing == 1){
									break;
								}
								close(pipefd[1]);
								newfd = pipefd[0];
							}
						}
						//piping is done 

					}
					else { //>> no piping
					
						char * args[128]; //holds each argument separated by spaces
						char * currcmdBuf = (char *)malloc((strlen(cmd)+1)*sizeof(char));
						strcpy(currcmdBuf, cmd);	//holds the command to be run
				
						char * currcmdBuf2 = strtok_r(currcmdBuf, ">", &currcmdBuf);
						char * currcmdBuf3 = (char *)malloc((strlen(currcmdBuf2)+1)*sizeof(char));
						strcpy(currcmdBuf3, currcmdBuf2);
						currcmdBuf2 = strtok_r(currcmdBuf, ">", &currcmdBuf);
						char * redir = (char *)malloc((strlen(currcmdBuf2)+1)*sizeof(char));
						strcpy(redir, currcmdBuf2);
						char * redir2 = redir;
						redir = strtok_r(redir2, " ", &redir2);
						
						char * currcmd = strtok(currcmdBuf3, " \n");	//get the command here 
						
						char * cmdToExec = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
						strcpy(cmdToExec, currcmd);	//store the command in a string
			
			
						int count = 0;
						while (currcmd != NULL) { //populates the args array
							args[count] = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
							strcpy(args[count], currcmd);
							args[count+1] = NULL;
							currcmd = strtok(NULL, " \n");
				
							count++;
						}

						
						//to run a command
						pid = fork();
	
						if (pid == 0) {
							
							redirection(redir, 2);
							execvp(cmdToExec, args);
							close(fd);
							return 0;
						} else {
							wait(NULL);
							if (killing == 1){
								killing = 0;
								break;
							}
							pid = 0;
						}
						pid = 0;
						//to run a command

					}


				}
				else { //this means ">" //use mode 1
					//check for piping and run the command and redirect the output 
					if (strstr(cmd, "|") != NULL){ //here we can use strtok_r
						//printf("piping with redirection\n");

						char *forRedir = (char *)malloc((strlen(cmd) + 1) * sizeof(char));
						strcpy(forRedir, cmd);
						char *redir2 = forRedir;
						char *redir = strtok_r(redir2, ">", &redir2);// now you have the first token
						char *token1 = (char *)malloc((strlen(redir) + 1) * sizeof(char));
						strcpy(token1, redir);
						redir = strtok_r(redir2, " >", &redir2);//now you have the second token


						//now you deal with piping in the first token
						char *pipeBuffer = token1;
						char *newCmd;
						


						int pipefd[2];
						pid_t pipepid;
						int newfd = 0;

						while ((newCmd = strtok_r(pipeBuffer, "|", &pipeBuffer))){ //for each pipe encountered till end of the string 
							
							
							char * args[128]; //holds each argument separated by spaces
							char * currcmdBuf = (char *)malloc((strlen(newCmd)+1)*sizeof(char));
							strcpy(currcmdBuf, newCmd);	//holds the command to be run
							char * currcmd = strtok(currcmdBuf, " \n");	//get the command here 
							//creating the pointers that will hold the arguments and executed command

							
							//creating the pointers that will hold the arguments and executed command
							char * cmdToExec = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
							strcpy(cmdToExec, currcmd);	//store the command in a string
							
										
					
							int count = 0;
							while (currcmd != NULL) { //populates the args array
								
								args[count] = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
								strcpy(args[count], currcmd);
								args[count+1] = NULL;
								currcmd = strtok(NULL, " \n");
						
								count++;
							}

							pipe(pipefd);
							pid = fork();

							if (pid == -1) {
								perror("error");
								exit(1);
							} else if (pid == 0) {
								dup2(newfd, 0);
								if (strlen(pipeBuffer) != 0){
									dup2(pipefd[1], 1);
								}
								else {
									redirection(redir, 1);
								}
								close(pipefd[0]);
								execvp(cmdToExec, args);
								exit(1);
							} else {
								wait(NULL);
								if (killing == 1) {
									break;
								}
								close(pipefd[1]);
								newfd = pipefd[0];
							}
						}
						//piping is done 


					}
					else { //without piping 
						
						char * args[128]; //holds each argument separated by spaces
						char * currcmdBuf = (char *)malloc((strlen(cmd)+1)*sizeof(char));
						strcpy(currcmdBuf, cmd);	//holds the command to be run
				
						char * currcmdBuf2 = strtok_r(currcmdBuf, ">", &currcmdBuf);
						char * currcmdBuf3 = (char *)malloc((strlen(currcmdBuf2)+1)*sizeof(char));
						strcpy(currcmdBuf3, currcmdBuf2);
						currcmdBuf2 = strtok_r(currcmdBuf, ">", &currcmdBuf);
						char * redir = (char *)malloc((strlen(currcmdBuf2)+1)*sizeof(char));
						strcpy(redir, currcmdBuf2);
						char * redir2 = redir;
						redir = strtok_r(redir2, " ", &redir2);
						
						char * currcmd = strtok(currcmdBuf3, " \n");	//get the command here 
						
						char * cmdToExec = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
						strcpy(cmdToExec, currcmd);	//store the command in a string
			
			
						int count = 0;
						while (currcmd != NULL) { //populates the args array
							args[count] = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
							strcpy(args[count], currcmd);
							args[count+1] = NULL;
							currcmd = strtok(NULL, " \n");
				
							count++;
						}

						
						//to run a command
						pid = fork();
	
						if (pid == 0) {
							
							redirection(redir, 1);
							execvp(cmdToExec, args);
							close(fd);
							return 0;
						} else {
							wait(NULL);
							if (killing == 1) {
								killing = 0;
								break;
							}
							pid = 0;
						}
						pid = 0;
						//to run a command

					}

				}
				if (killing == 1) {
					killing = 0;
					break;
				}

				continue;
			}
			//handles redirection with or without piping



			//handles only piping if redirection was not found 
			if (strstr(cmd, "|") != NULL){ //here we can use strtok_r
				char *pipeBuffer = cmd;
				char *newCmd;
				


				int pipefd[2];
				pid_t pipepid;
				int newfd = 0;

				while ((newCmd = strtok_r(pipeBuffer, "|", &pipeBuffer))){ //for each pipe encountered till end of the string 
					
					
					char * args[128]; //holds each argument separated by spaces
					char * currcmdBuf = (char *)malloc((strlen(newCmd)+1)*sizeof(char));
					strcpy(currcmdBuf, newCmd);	//holds the command to be run
					char * currcmd = strtok(currcmdBuf, " \n");	//get the command here 
					//creating the pointers that will hold the arguments and executed command

					
					//creating the pointers that will hold the arguments and executed command
					char * cmdToExec = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
					strcpy(cmdToExec, currcmd);	//store the command in a string
					
			
					int count = 0;
					while (currcmd != NULL) { //populates the args array
						
						args[count] = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
						strcpy(args[count], currcmd);
						args[count+1] = NULL;
						currcmd = strtok(NULL, " \n");
				
						count++;
					}

					pipe(pipefd);
					pid = fork();
					
					if (pid == -1) {
						perror("error");
						exit(1);
					} else if (pid == 0) {
						dup2(newfd, 0);
						
						if (strlen(pipeBuffer) != 0){
							dup2(pipefd[1], 1);
							
						}
						close(pipefd[0]);
						execvp(cmdToExec, args);
						exit(1);
					} else {
						wait(NULL);
						if (killing == 1){
							break;
						}
						close(pipefd[1]);
						newfd = pipefd[0];
					}
				}
				if (killing == 1){
					killing = 0;
					break;
				}
				continue;
			}
			//handles only piping if redirection was not found





			//creating the pointers that will hold the arguments and executed command
			char * args[128]; //holds each argument separated by spaces
			char * currcmdBuf = (char *)malloc((strlen(cmd)+1)*sizeof(char));
			strcpy(currcmdBuf, cmd);	//holds the command to be run
			char * currcmd = strtok(currcmdBuf, " \n");	//get the command here 
			
			char * cmdToExec = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
			strcpy(cmdToExec, currcmd);	//store the command in a string
			
			
			int count = 0;
			while (currcmd != NULL) { //populates the args array
				args[count] = (char *)malloc((strlen(currcmd)+1)*sizeof(char));
				strcpy(args[count], currcmd);
				args[count+1] = NULL;
				currcmd = strtok(NULL, " \n");
				
				count++;
			}


			//to run a command
			pid = fork();
	
			if (pid == 0) {
				
				execvp(cmdToExec, args);
				return 0;
			} else {
				wait(NULL);
				if (killing == 1){
					killing = 0;
					break;
				}
				
				pid = 0;
			}
			pid = 0;
			//to run a command

			
		
		}
	}




	return 0;
}
