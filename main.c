/* edU */
/* Alberto Mosconi - 2020 */

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

        char *prova = "15c";
        int i1, i2, success;
        char c;

        success = sscanf(prova, "%d,%d%c", &i1, &i2, &c);

        switch (success)
        {
        case 1:
            sscanf(prova, "%d%c", &i1, &c);
            printf("%d %c %d", i1, c, success);
            break;
        case 3:
            printf("%d %d %c %d", i1, i2, c, success);
            break;

        default:
            printf("error\n");
            break;
        }
    }
    return 0;
}
