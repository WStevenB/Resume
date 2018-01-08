/* xsh_date.c - xsh_date */

#include <xinu.h>
#include <string.h>
#include <stdio.h>


shellcmd xsh_hello(int nargs, char *args[]) {

	/* Output info for '--help' argument */

	if (nargs == 2 && strncmp(args[1], "--help", 7) == 0) {
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tDisplays a welcome message with name of person\n");
		printf("\tAccepts exactly one user argument or throws error\n");
		return 0;
	}

	/* Check argument count */

	if (nargs != 2) {
		fprintf(stderr, "Error: %s accepts exactly one user argument\n", args[0]);
		return 1;
	}

	// display a welcome message

	else {
		printf("Hello %s, Welcome to the world of Xinu!!\n", args[1]);
		return 1;
	}


}
