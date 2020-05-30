#include <stdlib.h>
#include <stdio.h>
#define MAX_COMMAND_LENGTH 1024

int main(int argc, char const *argv[])
{
    char raw_input[MAX_COMMAND_LENGTH];

    while (1)
    {
        if (NULL == fgets(raw_input, sizeof(raw_input), stdin))
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
            printf("cambia testo\n");
            break;
        case 'd':
            printf("cancella testo\n");
            break;
        case 'p':
            printf("stampa testo\n");
            break;
        case 'u':
            printf("annullamento\n");
            break;
        case 'r':
            printf("redo\n");
            break;

        default:
            break;
        }
    }
    return 0;
}