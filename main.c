#include <stdlib.h>
#include <stdio.h>
#define MAX_COMMAND_LENGTH 1024

/* MAIN FUNCTIONS */
void getInput();
void handleChange();
void handleDelete();
void handlePrint();
void handleUndo();
void handleRedo();

/* STRUCTURES */
typedef struct l
{
    char content[MAX_COMMAND_LENGTH];
    struct l *prevVersion;
    struct l *nextVersion;
} Line;

typedef struct lh
{
    int index;
    Line *latestVersion;
    Line *currentVersion;
    struct lh *nextLine;
} LineHistory;

/* VARIABLES */
char raw_command[MAX_COMMAND_LENGTH];
int i1 = 0, i2 = 0;
char c;
LineHistory *head = NULL;

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
{
    printf("> ");
    if (NULL == gets(raw_command))
        return;

    int success = sscanf(raw_command, "%d,%d%c", &i1, &i2, &c);
    if (1 == success)
        sscanf(raw_command, "%d%c", &i1, &c);
    else if (0 == success)
        sscanf(raw_command, "%c", &c);
}

void handleChange()
{
    printf("CHANGE between %d and %d\n", i1, i2);
    return;
}

void handleDelete()
{
    printf("DELETE between %d and %d\n", i1, i2);
    return;
}

void handlePrint()
{
    printf("PRINT between %d and %d\n", i1, i2);

    for (LineHistory *temp = head; temp != NULL; temp = temp->nextLine)
        puts(temp->currentVersion->content);

    return;
}

void handleUndo()
{
    printf("UNDO %d times\n", i1);
    return;
}

void handleRedo()
{
    printf("REDO %d times\n", i1);
    return;
}