#include <stdio.h>
#include <errno.h>
#include "consts.h"

int main(int argc, char *argv[])
{
    int i;
    int num_threads = atoi(argv[2]);
    int num_counters = atoi(argv[3]);
    char file_num_name[13];
    FILE* commands_file;
    FILE* count_files[MAX_FILE_COUNTER];

    for (int i=0; i<num_counters; i++)
    {
        snprintf(file_num_name, 13, TREAD_FILE_NAME_TEMPLATE, i);
        count_files[i] = fopen(file_num_name, "w");
        if (count_files[i] == NULL)
	    {
            printf("%s failed, errno is %d\n", "waitpid", errno);
		    printf("Counter file isn't created. Please check and run again.");
		    exit(1);
	    }
        fprintf(count_files[i], "0");
        fflush(count_files[i]);
    }

    commands_file = fopen(argv[1], "r");
	
	// open input files
	if (commands_file == NULL)
	{
		printf("Commands file doesn't open. Please check and run again.");
		exit(1);
	}

    fclose(commands_file);
    for (int i=0; i<num_counters; i++)
    {
        fclose(count_files[i]);
    }

    exit(0);
}