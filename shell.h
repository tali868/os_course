#define MAX_LINE_LENGHT 50

#define EXIT "exit"
#define CD "cd"
#define JOBS "jobs"

typedef struct Instruction {
	char operation[4];
    char extra[MAX_LINE_LENGHT];
} Instruction;