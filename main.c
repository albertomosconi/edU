#include <stdlib.h>
#include <stdio.h>
#define MAX_ROW_LENGTH 1024

/* STRUCTURES */
struct Line
{ /* single node of the linked list representing the document */
    char content[MAX_ROW_LENGTH];
    struct Line *next;
};
struct Command
{ /* command object to be stored in the history stack */
    char type;
    int i1, i2;
    struct Command *prev; /* previous command in the stack*/
    struct Command *next; /* next command in the stack */
};

/* VARIABLES */
char raw_cmd[MAX_ROW_LENGTH]; /* command buffer */
int i1, i2;                   /* line indexes specified in the command */
char c;                       /* command character [q, c, d, p, u, r] */

/* MAIN FUNCTIONS */
void getInput();     /* receive and parse input commands from stdin */
void handleChange(); /* process change command */
void handleDelete(); /* procerss delete command */
void handlePrint();  /* process print command */
void handleUndo();   /* process undo command */
void handleRedo();   /* process redo command */

int main(int argc, char const *argv[])
{
    while (1)
    {
        getInput();
        switch (c)
        {
        case 'q':
            return 0;
        case 'c':
            handleChange();
            break;
        case 'd':
            handleDelete();
            break;
        case 'p':
            handlePrint();
            break;
        case 'u':
            handleUndo();
            break;
        case 'r':
            handleRedo();
            break;
        default:
            break;
        }
    }
    return 0;
}

void getInput()
{ /* receive and parse input commands from stdin */
    if (!fgets(raw_cmd, sizeof(raw_cmd), stdin))
        exit(1); /* if fail stop program */

    int success = sscanf(raw_cmd, "%d,%d%c", &i1, &i2, &c);
    if (1 == success)
        sscanf(raw_cmd, "%d%c", &i1, &c);
    else if (0 == success)
        sscanf(raw_cmd, "%c", &c);
}

void handleChange() {}

void handleDelete() {}

void handlePrint() {}

void handleUndo() {}

void handleRedo() {}
