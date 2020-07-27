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

/* VARIABLES */
char raw_input[MAX_COMMAND_LENGTH];
int i1 = 0, i2 = 0;
char c;

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
    i1 = 0, i2 = 0;
    c = NULL;

    printf("> ");
    if (NULL == gets(raw_input))
        return;

    int success = sscanf(raw_input, "%d,%d%c", &i1, &i2, &c);
    if (1 == success)
        sscanf(raw_input, "%d%c", &i1, &c);
    else if (0 == success)
        sscanf(raw_input, "%c", &c);
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