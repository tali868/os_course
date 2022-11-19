typedef int bool;
#define true 1
#define false 0

#define MAX_LINE_LENGHT 50

#define EXIT "exit"
#define CD "cd"
#define JOBS "jobs"

typedef struct Instruction {
    char* raw_instruction;
	char operation[4];
    bool backround;
    char* directory;
    bool invalid_argument;
} Instruction;

struct JobNode {
    long pid;
    char* raw_instruction;
    struct JobNode* next;
} JobNode;
