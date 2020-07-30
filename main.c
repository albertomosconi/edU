#include <stdlib.h>
#include <stdio.h>
#define MAX_ROW_LENGTH 1025

/* STRUCTURES */
struct Command
{ /* command object to be stored in the history stack */
    char c;
    int i1, i2;
    char **lines;         /* lines to be changed */
    struct Command *prev; /* previous command in the stack*/
    struct Command *next; /* next command in the stack */
};
struct Modifier
{
    int lim;               /* index above which the modifier must be applied */
    int val;               /* number of deleted lines */
    struct Modifier *next; /* next modifier */
};

/* VARIABLES */
char raw_cmd[MAX_ROW_LENGTH]; /* command buffer */
int i1, i2;                   /* line indexes specified in the command */
char c;                       /* command character [q, c, d, p, u, r] */
struct Command *command_history = NULL;
struct Modifier *mods = NULL;

/* MAIN FUNCTIONS */
void getInput();     /* receive and parse input commands from stdin */
void handleChange(); /* process change command */
void handleDelete(); /* procerss delete command */
void handlePrint();  /* process print command */
void handleUndo();   /* process undo command */
void handleRedo();   /* process redo command */

/* HELPER FUNCTIONS */
void cmdToHistory();
int max(int a, int b); /* returns maximum between a and b */
int min(int a, int b); /* returns minimum between a and b */
void updateIndexes();  /* applies all the modifiers to the indexes */

int main()
{
    while (1)
    {
        getInput(); /* fetch and parse input */
        switch (c)
        {
        case 'q': /* terminate the program */
            return 0;
        case 'c': /* change lines */
            handleChange();
            break;
        case 'd': /* delete lines */
            handleDelete();
            break;
        case 'p': /* print lines */
            handlePrint();
            break;
        case 'u': /* undo commands */
            handleUndo();
            break;
        case 'r': /* redo commands */
            handleRedo();
            break;
        default: /* unknown command */
            break;
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

int max(int a, int b)
{ /* returns maximum between a and b */
    if (a >= b)
        return a;
    return b;
}

int min(int a, int b)
{ /* returns minimum between a and b */
    if (a > b)
        return b;
    return a;
}

void updateIndexes()
{ /* applies all the modifiers to the indexes */
}

void getInput()
{ /* receive and parse input commands from stdin */
    // fputs("> ", stdout);
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
    updateIndexes(); /* update the indexes with the modifiers */

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
    command_history->lines = malloc((i2 - i1 + 1) * sizeof(char *));
    for (int k = 0; k < i2 - i1 + 1; ++k)
    {
        command_history->lines[k] = malloc(MAX_ROW_LENGTH * sizeof(char));
        fgets(command_history->lines[k], MAX_ROW_LENGTH * sizeof(char), stdin);
    }
    command_history->next = NULL;
}

void handleDelete()
{
    updateIndexes(); /* update the indexes with the modifiers */

    /* add a modifier to the list */
}

void handlePrint()
{
    char **buffer = malloc((i2 - i1 + 1) * sizeof(char *));
    int *found = calloc((i2 - i1 + 1), sizeof(int));
    int to_find = i2 - i1 + 1; /* how many elements are still to be found */
    int len = to_find;

    updateIndexes(); /* update the indexes with the modifiers */

    struct Command *curr_cmd = command_history;
    while ((curr_cmd != NULL) && (to_find > 0))
    {
        if ((i1 <= curr_cmd->i2) && (i2 >= curr_cmd->i1))
        {
            /* save lines from max(i1, curr_cmd->i1) to min(i2, curr_cmd->i2) */
            for (int k = max(i1 - curr_cmd->i1, 0); k < min(i2, curr_cmd->i2) - curr_cmd->i1 + 1; ++k)
            {
                if (!found[k + (curr_cmd->i1 - i1)])
                {
                    buffer[k + (curr_cmd->i1 - i1)] = curr_cmd->lines[k];
                    found[k + (curr_cmd->i1 - i1)] = 1;
                    --to_find;
                }
            }
        }
        curr_cmd = curr_cmd->prev;
    }

    /* print the buffer */
    for (int k = 0; k < len; ++k)
    {
        if (!found[k])
            fputs(".\n", stdout);
        else
            fputs(buffer[k], stdout);
    }

    free(buffer);
    free(found);
}

void handleUndo()
{
    printf("undo\n");
}

void handleRedo()
{
    printf("redo\n");
}
