#include "consts.h"
#include "parsing.h"

bool is_worker(char *line){
	char *worker = "worker";
	// Check if the first 5 characters of the line are "worker"
	if(strncmp(line, worker, 5) == 0){
		return true;
	}
	return false;
}

bool is_dispatcher(char *line){
	char *dispatcher = "dispatcher";
	// Check if the first 9 characters of the line are "dispatcher"
	if(strncmp(line, dispatcher, 9) == 0){
		return true;
	}
	return false;
}


char *mystrtok(char *str, char *delimiters)
{
    static char *string;
    int i, j;
 
    if (str != NULL)
    {
        string = str;
    }
 
    if (string == NULL)
    {
        return string;
    }
 
    char *token = (char *)malloc(strlen(string)+1);
 
    for (i = 0; string[i] != '\0'; i++)
    {
        int flag = 0;
        for (j = 0; delimiters[j] != '\0'; j++)
        {
            if (string[i] == delimiters[j])
            {
                flag = 1;
            }
        }
 
        if (flag == 0)
        {
            token[i] = string[i];
        }
        else
        {
            token[i] = '\0';
            string = string + i + 1;
            return token;
        }
    }
 
    token[i] = '\0';
    string = NULL;
    return token;
}
