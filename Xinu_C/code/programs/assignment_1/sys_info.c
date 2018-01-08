
#include <stdlib.h>	
#include <unistd.h>	
#include <stdio.h>
#include <string.h>	

int main(int argc, char *argv[]) {
	
	//check number of arguments
	if(argc != 2) {
		printf("Error - must provide exactly one user argument\n");
		return 1;
	}
	
	//check argument length
	if(strlen(argv[1]) > 10) {
		printf("Error - argument must be less than 11 characters\n");
		return 1;
	}

	//prepare pipe
	int data[2];
	pipe(data);

	//fork a new process
	int pid = fork();
	
	//test which process we're in
	switch (pid) {
	
	case -1:	//error
		printf("Failed to create second process\n");
		break;
	
	case 0:		//child
	
		printf("Child PID = %d\n", getpid());
		
		//read from pipe
		char buff[10];
		close(data[1]);
		read(data[0], buff, 10);
		
		//prepare command line arg
		char command[20];
    	strcpy(command, "/bin/");
    	strcat(command, buff);
		
		//run execl with arg
		if(strncmp(buff, "echo", 4) == 0) execl(command, command, "Hello World!", NULL);
		else execl(command, command, NULL);
		break;

	default:	//parent
		printf("Parent PID = %d\n", getpid());
		
		//write to pipe
		close(data[0]);
		write(data[1], argv[1], 10);
		
		//wait for child to exit
		int returnStatus;    
    	waitpid(pid, &returnStatus, 0);
		break;
	}
	
	return 1;
}

