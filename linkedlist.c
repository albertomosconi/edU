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
int i1, i2;
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
    // printf("> ");
    if (!fgets(raw_command, sizeof raw_command, stdin))
        return;

    int success = sscanf(raw_command, "%d,%d%c%*c", &i1, &i2, &c);
    if (1 == success)
        sscanf(raw_command, "%d%c%*c", &i1, &c);
    else if (0 == success)
        sscanf(raw_command, "%c%*c", &c);
}

void handleChange()
{
    // printf("CHANGE between %d and %d\n", i1, i2);

    if (!head) /* if the document is empy initialize the first line */
        head = initializeLineHistory(1);

    LineHistory *currentLine = head;
    for (int k = 1; k <= i2; ++k)
    {
        if (k >= i1)
        { /* create a new version of the line and add it to the history */
            currentLine->latestVersion = malloc(sizeof(Line));
            currentLine->latestVersion->nextVersion = NULL;
            currentLine->latestVersion->prevVersion = currentLine->currentVersion;
            currentLine->currentVersion = currentLine->latestVersion;
            fgets(currentLine->currentVersion->content, sizeof(raw_command), stdin);
        }
        if (k != i2)
        { /* if null initialize the next line until last element */
            if (NULL == currentLine->nextLine)
                currentLine->nextLine = initializeLineHistory(k + 1);
            currentLine = currentLine->nextLine;
        }
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
    // printf("PRINT between %d and %d\n", i1, i2);

    LineHistory *currentLine = head;

    if (i1 == 0)
        fputs(".\n", stdout);

    for (int k = 1; (k < i1) && (currentLine != NULL); ++k)
        currentLine = currentLine->nextLine;

    for (currentLine; (currentLine != NULL) && (currentLine->index <= i2); currentLine = currentLine->nextLine)
    {
        // printf("%d ", currentLine->index);
        fputs(currentLine->currentVersion->content, stdout);
    }

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