typedef int bool;
#define true 1
#define false 0

#define MAX_LINE_LENGHT 100

typedef enum {
    CD,
    JOBS,
    EXIT,
    INST
} InstructionType;

typedef struct Instruction {
    char* raw_instruction;
	InstructionType operation;
    bool is_backround;
    char* directory;
} Instruction;

typedef struct _JobNode {
    long pid;
    char* raw_instruction;
    bool is_active;
} JobNode;
