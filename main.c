#include <stdlib.h>
#include <stdio.h>
#define MAX_COMMAND_LENGTH 1024

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

/* MAIN FUNCTIONS */
void getInput();
void handleChange();
void handleDelete();
void handlePrint();
void handleUndo();
void handleRedo();

/* HELPER FUNCTIONS*/
void copyString(char *source, char *destination);
LineHistory *initializeLineHistory(int i);

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

void copyString(char *source, char *destination)
{
    for (int c = 0; source[c] != '\0'; ++c)
        destination[c] = source[c];
    destination[c] = '\0';
}

LineHistory *initializeLineHistory(int i)
{
    Line line;
    copyString(".", line.content);
    line.nextVersion = NULL;
    line.prevVersion = NULL;

    LineHistory *new = malloc(sizeof(LineHistory));
    new->index = i;
    new->nextLine = NULL;
    new->latestVersion = &line;
    new->currentVersion = &line;

    return new;
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

    if (!head)
        head = initializeLineHistory(1);

    LineHistory *currentLine = head;

    for (int k = 2; k < i1; ++k)
    {
        if (currentLine->nextLine == NULL)
            currentLine->nextLine = initializeLineHistory(k);
        currentLine = currentLine->nextLine;
    }

    for (int k = i1; k <= i2; ++k)
    {
        if (currentLine->nextLine == NULL)
            currentLine->nextLine = initializeLineHistory(k);

        currentLine->nextLine->currentVersion = malloc(sizeof(Line));
        currentLine->nextLine->latestVersion = currentLine->nextLine->currentVersion;
        gets(currentLine->nextLine->currentVersion->content);
        currentLine->nextLine->currentVersion->prevVersion = currentLine->nextLine->currentVersion;
        currentLine->nextLine->currentVersion->nextVersion = NULL;

        currentLine = currentLine->nextLine;
    }

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

    LineHistory *currentLine = head;

    for (int k = 1; (k < i1) && (currentLine != NULL); ++k)
        currentLine = currentLine->nextLine;

    for (currentLine; (currentLine != NULL) && (currentLine->index <= i2); currentLine = currentLine->nextLine)
        puts(currentLine->currentVersion->content);

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