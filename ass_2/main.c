#include <stdio.h>
#include "consts.h"

int main(int argc, char *argv[])
{
    int i;
    int num_threads = atoi(argv[2]);
    int num_counters = atoi(argv[3]);
    FILE* commands_file;
    FILE *fptr;

    commands_file = fopen(argv[1], "r");
	
	// open input files
	if (commands_file == NULL)
	{
		printf("Commands file doesn't open. Please check and run again.");
		exit(1);
	}

    fclose(commands_file);

    exit(0);
}