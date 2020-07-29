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
    char c;
    int i1, i2;
    struct Command *prev; /* previous command in the stack*/
    struct Command *next; /* next command in the stack */
};

/* VARIABLES */
char raw_cmd[MAX_ROW_LENGTH]; /* command buffer */
int i1, i2;                   /* line indexes specified in the command */
char c;                       /* command character [q, c, d, p, u, r] */
struct Command *command_history = NULL;

/* MAIN FUNCTIONS */
void getInput();     /* receive and parse input commands from stdin */
void handleChange(); /* process change command */
void handleDelete(); /* procerss delete command */
void handlePrint();  /* process print command */
void handleUndo();   /* process undo command */
void handleRedo();   /* process redo command */

/* HELPER FUNCTIONS */
void cmdToHistory();

int main()
{
    while (1)
    {
        getInput();
        switch (c)
        {
        case 'q': /* terminate the program */
            exit(0);
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
            exit(1);
        }
    }
    return 0;
}

void cmdToHistory()
{ /* add the command to the history */
    if (!command_history)
    {
        command_history = malloc(sizeof(struct Command));
        command_history->prev = NULL;
    }
    else
    {
        command_history->next = malloc(sizeof(struct Command));
        command_history->next->prev = command_history;
        command_history = command_history->next;
    }
    command_history->c = c;
    command_history->i1 = i1;
    command_history->i2 = i2;
    command_history->next = NULL;
}

void getInput()
{ /* receive and parse input commands from stdin */
    fputs("> ", stdout);
    if (!fgets(raw_cmd, sizeof(raw_cmd), stdin))
        exit(1); /* if fail stop program */

    int success = sscanf(raw_cmd, "%d,%d%c", &i1, &i2, &c);
    if (1 == success)
    {
        sscanf(raw_cmd, "%d%c", &i1, &c);
        i2 = -1;
    }
    else if (0 == success)
    {
        sscanf(raw_cmd, "%c", &c);
        i1 = -1;
        i2 = -1;
    }
}

void handleChange()
{
    cmdToHistory();
}

void handleDelete()
{
    cmdToHistory();
}

void handlePrint()
{
    struct Command *temp = command_history;
    while (NULL != temp)
    {
        printf("%d,%d%c\n", temp->i1, temp->i2, temp->c);
        temp = temp->prev;
    }
}

void handleUndo()
{
    printf("undo\n");
}

void handleRedo()
{
    printf("redo\n");
}
