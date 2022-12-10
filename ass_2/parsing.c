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
