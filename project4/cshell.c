#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


int exiting = 0;
pid_t pid = 0;

void ctrlCSignal(int signum){
	printf("makes it inside ;)\n");
    
	if (pid != 0){
   		kill(pid, 0);
	}
}	




int main(int argc, char *argv[]){

	signal(SIGINT, ctrlCSignal);

	while (exiting == 0) {

		//gets the current working directory to set up prompt
   		char currDirr[1000];
    	getcwd(currDirr, 1000);
    	printf("%s $ ", currDirr);

		//gets an input command from stdin
		char buf[500];
        fgets(buf, 500, stdin);


        char * cmd = strtok(buf, ";");
		
		while (cmd != NULL){
			
			printf("%s\n", cmd);

			////for exit
			int index = 0;
			for (int i = 0; i < strlen(cmd); i++){
				index = i;
				if (cmd[i] != ' '){
					break;
				}
			}
			if ((cmd[index] == 'e' ) && (strstr(cmd, "exit\n") != NULL|| strstr(cmd, "exit ") != NULL)){
				printf("hi\n");
				return 0;
			}
            ////for exit


			  

/*
            cat file1.txt file2.txt

            char * args[] = {"cat", "file1.txt", "file2.txt", (char*)0};
            execvp(args[0], args);


			pid = fork();
            if (pid = fork() == 0){

				
				
				execvp();
			}
			else {
				wait(NULL);
				pid = 0;
			}

*/





			cmd = strtok(NULL, " ; ");
		}
        

	}




	return 0;
}