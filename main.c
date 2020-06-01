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
    int current, end;
} history;

int main(int argc, char const *argv[])
{
    char raw_input[MAX_COMMAND_LENGTH];
    history document;
    document.end = -1;
    document.current = -1;

    while (1)
    {
        printf("> ");
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
            if (document.current < document.end)
                document.end = document.current;

            state new_change;
            new_change.tot_lines = 0;
            if (document.end > -1)
                new_change = document.list[document.end];

            for (int i = i1 - 1; i < i2; ++i)
            {
                gets(new_change.file[i]);
            }

            if (i2 > new_change.tot_lines)
                new_change.tot_lines = i2;

            document.list[++document.end] = new_change;
            ++document.current;
            break;
        case 'd':
            printf("Cancella Testo\n");
            break;
        case 'p':
            for (int i = i1 - 1; (i < i2) && (i < document.list[document.current].tot_lines); ++i)
            {
                if ('\0' != document.list[document.current].file[i][0])
                    puts(document.list[document.current].file[i]);
                else
                    puts(".");
            }
            break;
        case 'u':
            printf("Undoing %d commands\n", i1);
            if (i1 > document.current)
                document.current = -1;
            else
                document.current -= i1;
            break;
        case 'r':
            printf("Redoing %d commands\n", i1);
            if (document.current + i1 > document.end)
                document.current = document.end;
            else
                document.current += i1;
            break;
        default:
            printf("Current Version: %d\nLength: %d\nLast Version: %d\nLenght: %d\n", document.current + 1, document.list[document.current].tot_lines, document.end + 1, document.list[document.end].tot_lines);
            break;
        }
    }
    return 0;
}