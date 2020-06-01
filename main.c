#include <stdlib.h>
#include <stdio.h>
#define MAX_COMMAND_LENGTH 1024

typedef struct
{
    int tot_lines;
    char file[10][1024];
} state;

typedef struct
{
    state list[10];
    int last;
} history;

int main(int argc, char const *argv[])
{
    char raw_input[MAX_COMMAND_LENGTH];
    history document;
    document.last = -1;

    while (1)
    {
        // printf("> ");
        if (NULL == gets(raw_input))
            break;

        int i1 = 0, i2 = 0;
        char c;

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
            // printf("editing %d lines\n", (i2 - i1 + 1));
            {
                state new_change;
                new_change.tot_lines = 0;
                if (document.last > -1)
                    new_change = document.list[document.last];

                for (int i = i1 - 1; i < i2; ++i)
                {
                    gets(new_change.file[i]);
                }

                if (i2 > document.list[document.last].tot_lines)
                    new_change.tot_lines = i2;

                document.list[++document.last] = new_change;
            }
            break;
        case 'd':
            printf("Cancella Testo\n");
            break;
        case 'p':
            // printf("printing lines %d to %d\n", i1, i2);
            for (int i = i1 - 1; (i < i2) && (i < document.list[document.last].tot_lines); ++i)
            {
                if ('\0' != document.list[document.last].file[i][0])
                    puts(document.list[document.last].file[i]);
                else
                    puts(".");
            }
            break;
        case 'u':
            printf("Undoing %d commands\n", i1);
            if (i1 > document.last)
                document.last = -1;
            else
                document.last -= i1;
            break;
        case 'r':
            printf("Redo\n");
            break;
        default:
            break;
        }
    }
    return 0;
}