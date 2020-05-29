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

        printf("%s", raw_input);
    }
    return 0;
}
