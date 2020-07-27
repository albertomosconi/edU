#include <stdlib.h>
#include <stdio.h>
#define MAX_COMMAND_LENGTH 1024

int main(int argc, char const *argv[])
{
    char raw_input[MAX_COMMAND_LENGTH];

    while (1)
    {
        printf("> ");
        if (NULL == gets(raw_input))
            break;

        int i1 = 0, i2 = 0;
        char c = 'q';

        int success = sscanf(raw_input, "%d,%d%c", &i1, &i2, &c);
        if (1 == success)
            sscanf(raw_input, "%d%c", &i1, &c);
        else if (0 == success)
            sscanf(raw_input, "%c", &c);

        switch (c)
        {
        case 'q':
            return 0;

        case 'c':
            printf("CHANGE between %d and %d\n", i1, i2);
            break;

        case 'd':
            printf("DELETE between %d and %d\n", i1, i2);
            break;

        case 'p':
            printf("PRINT between %d and %d\n", i1, i2);
            break;

        case 'u':
            printf("UNDO %d times\n", i1);
            break;

        case 'r':
            printf("REDO %d times\n", i1);
            break;

        default:
            printf("UNKNOWN command\n");
            break;
        }
    }
    return 0;
}