/* edU - Progetto Finale per l'esame di Algoritmi e Principi dell'informatica
 * 2019-2020, Politecnico di Milano
 * Alberto Mosconi */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* CONSTANTS */
#define MAX_ROW_LENGTH 250  /* maximum length of input strings */
#define MAX_CMD_LENGTH 100  /* maximum length of cmd strings */
#define INIT_ALLOC_SIZE 200 /* initial allocation size for various arrays */
#define REALLOC_FACTOR 4    /* multiplication factor for the size of the array being reallocated */
/* COMAND CHARACTERS */
#define QUIT_C 'q'   /* quit character */
#define CHANGE_C 'c' /* change character */
#define DELETE_C 'd' /* delete character */
#define PRINT_C 'p'  /* print character */
#define UNDO_C 'u'   /* undo character */
#define REDO_C 'r'   /* redo character */
#define TERM_C '.'   /* termination character */
/* STRUCTURES */
struct StringNode /* node in the strings tree */
{
    struct StringNode *parent;   /* parent node */
    struct StringNode *left;     /* left node */
    struct StringNode *right;    /* right node */
    int color;                   /* node color, 1->red, 0->black */
    char string[MAX_ROW_LENGTH]; /* string contents */
};
struct Command /* command node to be stored in the history stack */
{
    struct StringNode **lines; /* list of pointers to the lines in the tree */
    struct Command *prev;      /* previous command in the stack*/
    struct Command *next;      /* next command in the stack */
    int *mod_index;            /* list of modified indexes for saved lines */
    int i1, i2, mi1, mi2;      /* original indexes and modified indexes */
    int oldDocumentLength;     /* length of document before application of command */
    char c;                    /* command character */
};
/* GENERAL VARIABLES */
char raw_cmd[MAX_CMD_LENGTH];           /* command buffer */
char c;                                 /* command character [q, c, d, p, u, r] */
int i1, i2;                             /* line indexes specified in the command */
int *mods = NULL;                       /* array of modifiers */
int modlen = 0;                         /* line number after which all mod values are the same */
int modsBufferSize = 0;                 /* size of the modifiers array */
int apply = 0;                          /* if 1 applies consecutive undos and redos at once */
int undoBuffer = 0;                     /* when positive -> how many undos to apply, when negative -> redos */
int documentLength = 0;                 /* total number of lines in the document */
int totCommands = 0, currCommands = 0;  /* tot number of commands executed and current number of commands */
int *found = NULL;                      /* buffer array to keep track of lines found when printing */
int foundBufferSize = 0;                /* size of found array */
int printBufferSize = 0;                /* size of print buffer */
struct StringNode **printBuffer = NULL; /* array to hold pointers to the strings to print; */
struct StringNode *strings = NULL;      /* root of red black tree containing all the strings */
struct Command *latest_command = NULL;  /* pointer to latest command */
struct Command *current_command = NULL; /* pointer to current command */
/* MAIN FUNCTIONS */
void getInput();     /* receive and parse input commands from stdin */
void handleChange(); /* process change command */
void handleDelete(); /* procerss delete command */
void handlePrint();  /* process print command */
void handleUndo();   /* process undo command */
void handleRedo();   /* process redo command */
/* HELPER FUNCTIONS */
int max(int a, int b);        /* returns maximum between a and b */
int min(int a, int b);        /* returns minimum between a and b */
void findLines(int a, int b); /* fills the print buffer array with the lines between two indexes */
void documentInit();          /* initialize document */
void cmdToHistory();          /* adds a new change or delete command to the history */
void applyUndos();            /* trigger application of grouped undos and redos */
/* STRINGS TREE FUNCTIONS */
struct StringNode *insertString(); /* gets new line from stdin and inserts in memory (if it doesn't exist already) and returns a pointer to it */
struct StringNode *treeInsert(
    struct StringNode *root, struct StringNode *n, struct StringNode **s); /* if it doesnt exist, insert n in the strings tree, save pointer to it in s */
void rotateLeft(struct StringNode *n);                                     /* rotates the string tree to the left */
void rotateRight(struct StringNode *n);                                    /* rotates the string tree to the right */
void treeFixup(struct StringNode *n);                                      /* fix colors of tree */

int main()
{
    documentInit(); /* initialize some global variables */
    while (1)
    {
        getInput(); /* fetch and parse input */
        switch (c)  /* handle different commands */
        {
        case QUIT_C: /* terminate the program */
            return 0;
        case CHANGE_C: /* change lines */
            handleChange();
            break;
        case DELETE_C: /* delete lines */
            handleDelete();
            break;
        case PRINT_C: /* print lines */
            handlePrint();
            break;
        case UNDO_C: /* undo commands */
            handleUndo();
            break;
        case REDO_C: /* redo commands */
            handleRedo();
            break;
        default: /* unknown command */
            break;
        }
    }
    return 0;
}
void getInput()
{ /* receive and parse input commands from stdin */
    int i = -1;
    do
    {
        ++i;
        raw_cmd[i] = getchar_unlocked();
    } while ((i < MAX_CMD_LENGTH - 1) && (raw_cmd[i] != '\n'));
    raw_cmd[i + 1] = '\0';
    /* success hold the number of variables parsed correctly, so if it's 3 it means that 
        the input command was either a change, a delete or a print */
    int success = sscanf(raw_cmd, "%d,%d%c", &i1, &i2, &c);
    switch (success)
    {
    case 3: /* already parsed correctly */
        break;
    case 1: /* with only one digits it's either undo or redo */
        sscanf(raw_cmd, "%d%c", &i1, &c);
        i2 = -1;
        break;
    case 0: /* only one character, it's 'q' or '.' */
        sscanf(raw_cmd, "%c", &c);
        i1 = -1;
        i2 = -1;
        break;
    default: /* if it's any value different from 3, 1 or 0 it means the command is not supported */
        exit(1);
        break;
    }
    if ((c != UNDO_C) && (c != REDO_C) && (c != QUIT_C) && (c != TERM_C))
        applyUndos();
}
void handleChange() /* process change command */
{
    cmdToHistory(); /* add a new command struct to the list */
    int totRighe = i2 - i1 + 1;
    latest_command->lines = malloc(totRighe * sizeof(*(latest_command->lines)));
    latest_command->mod_index = malloc(totRighe * sizeof(*(latest_command->mod_index)));
    for (int k = 0; k < totRighe; ++k)
    { /* insert the new content */
        latest_command->lines[k] = insertString();
        latest_command->mod_index[k] = i1 + k + mods[min(i1 + k, modlen) - 1];
    }
    latest_command->next = NULL;
    current_command = latest_command;

    /* update document length */
    if (i2 > documentLength)
        documentLength = i2;
}
void handleDelete() /* process delete command */
{
    cmdToHistory(); /* add a new command struct to the list */
    if (i1 <= documentLength)
    {
        if (i1 >= modlen)
        { /* if the first deleted line is not in mods, make it bigger and copy old values */
            if (i1 > modsBufferSize)
            { /* if the modifier array is not big enough, reallocate it */
                modsBufferSize = max(i1, modsBufferSize * REALLOC_FACTOR);
                int *temp = realloc(mods, modsBufferSize * sizeof(*temp));
                if (temp != NULL)
                    mods = temp;
            }
            modlen = i1;
        }
        int b = i2;
        if (i2 > documentLength)
        { /* if deleting lines beyond end of document, adjust to delete until last one */
            b = documentLength;
            current_command->i2 = documentLength;
            /* update document length */
            documentLength -= (i2 - i1 + 1 - i2 + documentLength);
        }
        else /* update document length */
            documentLength -= i2 - i1 + 1;

        for (int k = i1 - 1; k < modlen; ++k)
            /* update new values */
            mods[k] += b - i1 + 1;
    }
    else /* if deleting lines beyond the end of the document, the command has no effect */
        current_command->i1 = -1;
}
void handlePrint() /* process print command */
{
    if (i1 > documentLength)
        /* if the first line to print is beyond the length of the document, no need to search for anything */
        for (int k = 0; k < i2 - i1 + 1; ++k)
            fputs(".\n", stdout);
    else
    { /* print the buffer */
        int b = i2;
        if (i2 > documentLength)
            /* if the last line to print is greater than the length of the document, search lines until end of document and not further */
            b = documentLength;

        int to_print = b - i1 + 1; /* number of lines to be printed */
        if (to_print > printBufferSize)
        { /* if the print buffer is not big enough to hold the needed lines */
            printBufferSize = max(b - i1 + 1, printBufferSize * REALLOC_FACTOR);
            /* reallocate print buffer */
            struct StringNode **tempP = realloc(printBuffer, printBufferSize * sizeof(*tempP));
            if (tempP != NULL)
                printBuffer = tempP;

            foundBufferSize = max(to_print, foundBufferSize * REALLOC_FACTOR);
            /* reallocate found buffer */
            int *tempF = calloc(foundBufferSize, sizeof(*tempF));
            if (tempF != NULL)
            {
                free(found);
                found = tempF;
            }
        }
        /* fill the buffer with the needed lines */
        findLines(i1, b);
        /* print the buffer */
        for (int k = 0; k < to_print; ++k)
            if (printBuffer[k] == NULL)
                fputs(".\n", stdout);
            else
                fputs(printBuffer[k]->string, stdout);
        /* print remaining lines beyond the end of document if needed */
        for (int k = 0; k < i2 - b; ++k)
            fputs(".\n", stdout);
    }
}
void handleUndo() /* process undo command */
{
    if (apply != 0)
    { /* apply grouped undos */
        while ((current_command != NULL) && (current_command->prev != NULL) && (undoBuffer > 0))
        { /* go back 'undoBuffer' number of times, or until it is empty, in the command list */
            if ((current_command->c == DELETE_C) && (current_command->i1 != -1))
            { /* if undoing a delete command, adjust index modifiers */
                for (int k = current_command->i1 - 1; k < modlen; ++k)
                    mods[k] -= current_command->i2 - current_command->i1 + 1;
            }
            /* update document length */
            documentLength = current_command->oldDocumentLength;
            current_command = current_command->prev;
            --undoBuffer;
            --currCommands;
        }
    }
    else
    { /* group undo with previous similar commands */
        if (currCommands - undoBuffer - i1 >= 0)
            /* if it doesn't go back before first version, add it to the buffer */
            undoBuffer += i1;
        else
            /* if it goes back before first version, set the buffer to the maximum numers of permitted undos 
            (number of commands executed until current version) */
            undoBuffer = currCommands;
    }
}
void handleRedo() /* process redo command */
{
    if (apply != 0)
    { /* apply grouped redos */
        undoBuffer *= -1;
        while ((current_command != NULL) && (current_command->next != NULL) && (undoBuffer > 0))
        { /* go forward 'undoBuffer' number of times, or until it is full, in the command list */
            current_command = current_command->next;
            if ((current_command->c == DELETE_C) && (current_command->i1 != -1))
            { /* if redoing a delete command, adjust index modifiers */
                for (int k = current_command->i1 - 1; k < modlen; ++k)
                    mods[k] += current_command->i2 - current_command->i1 + 1;
            }
            /* restore document length */
            if (current_command->i2 > current_command->oldDocumentLength)
                documentLength = current_command->i2;
            else
                documentLength = current_command->oldDocumentLength;

            --undoBuffer;
            ++currCommands;
        }
    }
    else
    { /* group redo with previous similar commands */
        if (currCommands - undoBuffer + i1 <= totCommands)
            /* if it doesn't go beyond latest version, subtract it from the buffer */
            undoBuffer -= i1;
        else /* if it goes beyond latest version, set the buffer to the maximum number of permitted redos
            (number of commands executed between the current version and the latest) */
            undoBuffer = currCommands - totCommands;
    }
}
void findLines(int a, int b) /* fills the print buffer array with the lines between two indexes */
{
    int to_find = b - a + 1; /* save the number of lines to be printed */
    /* traverse the command list backwards, until all lines are found or until the end */
    for (struct Command *curr_cmd = current_command; (curr_cmd->prev != NULL) && (to_find > 0); curr_cmd = curr_cmd->prev)
    { /* if current command is a change, it might contain some of the needed lines */
        if (curr_cmd->c == CHANGE_C)
        { /* if the index of the first line I need is less than the last index available in the current command,
            and if the last index I need if greater than the first index available in the current command,
            all adjusted with the modifiers in order to handle line deletions */
            if ((a + mods[min(a, modlen) - 1] <= curr_cmd->mod_index[curr_cmd->i2 - curr_cmd->i1]) && (b + mods[min(b, modlen) - 1] >= curr_cmd->mod_index[0]))
            { /* save the number of available lines in the current command */
                int localAvailable = curr_cmd->i2 - curr_cmd->i1 + 1;
                /* iterate over the needed indexes, starting from the start, if the current command has lines before that, or from the first line
                    in the current command, if the first line that's needed comes before the first available.
                    a few extra loops might happen because the indexes are not being adjusted to account for deletions */
                for (int j = max(0, curr_cmd->i1 - a); (j < b - a + 1) && (to_find > 0); ++j)
                { /* save the index of the current line to print, adjusted for deletions */
                    int curr_index = a + j + mods[min(a + j, modlen) - 1];
                    /* if the current line has not already been found, iterate over the lines available in the current command, 
                        starting from the first if if the first line to be printed comes before the first
                        available, or from the first line to be printed if it's after the first available in the command.
                        stop when no more lines in the current command are available */
                    for (int k = max(0, a - curr_cmd->i1); (found[j] == 0) && (k < curr_cmd->i2 - curr_cmd->i1 + 1) && (localAvailable > 0) && (to_find > 0); ++k)
                    { /* if the needed index and the current available index match, save the line in the buffer and mark it as found */
                        if (curr_index == curr_cmd->mod_index[k])
                        {
                            found[j] = 1;
                            printBuffer[j] = curr_cmd->lines[k];
                            --to_find;
                            --localAvailable;
                        }
                    }
                }
            }
        }
    }
    for (int k = 0; k < b - a + 1; ++k)
    { /* in case any line has not been found, set it as NULL */
        if (found[k] == 0)
            printBuffer[k] = NULL;
        /* reset the found buffer */
        found[k] = 0;
    }
}
void documentInit() /* initialize document */
{
    /*  initialize index modifiers array */
    mods = calloc(INIT_ALLOC_SIZE, sizeof(*mods));
    modlen = 1;
    modsBufferSize = INIT_ALLOC_SIZE;
    /* initialize found buffer array */
    found = calloc(INIT_ALLOC_SIZE, sizeof(*found));
    foundBufferSize = INIT_ALLOC_SIZE;
    /* initialize print buffer array */
    printBuffer = malloc(INIT_ALLOC_SIZE * sizeof(*printBuffer));
    printBufferSize = INIT_ALLOC_SIZE;
    /* initialize command history */
    latest_command = malloc(sizeof(*latest_command));
    latest_command->c = '#';
    latest_command->i1 = -1;
    latest_command->i2 = -1;
    latest_command->oldDocumentLength = 0;
    latest_command->prev = NULL;
    current_command = latest_command;
}
void cmdToHistory() /* adds a new change or delete command to the history */
{
    /* delete previously undone commands, current command becomes latest command */
    while (latest_command != current_command)
    {
        latest_command = latest_command->prev;
        free(latest_command->next);
    }
    /* add command to history */
    latest_command->next = malloc(sizeof(*latest_command));
    latest_command->next->prev = latest_command;
    latest_command = latest_command->next;
    latest_command->next = NULL;
    latest_command->c = c;
    latest_command->i1 = i1;
    latest_command->i2 = i2;
    latest_command->mi1 = mods[min(i1, modlen - 1)];
    latest_command->mi2 = mods[min(i2, modlen - 1)];
    latest_command->oldDocumentLength = documentLength;
    current_command = latest_command;
    ++currCommands;
    totCommands = currCommands;
}
void applyUndos() /* trigger application of grouped undos and redos */
{
    if (undoBuffer != 0)
        apply = 1;
    if (undoBuffer > 0)
        handleUndo();
    else if (undoBuffer < 0)
        handleRedo();
    undoBuffer = 0;
    apply = 0;
}
int max(int a, int b) /* returns maximum between a and b */
{
    if (a >= b)
        return a;
    return b;
}
int min(int a, int b) /* returns minimum between a and b */
{
    if (a > b)
        return b;
    return a;
}
void rotateLeft(struct StringNode *n) /* rotates the string tree to the left */
{
    struct StringNode *n_right = n->right;
    struct StringNode *p = n->parent;

    n->right = n_right->left;

    if (n->right != NULL)
        n->right->parent = n;

    n_right->parent = p;

    if (p == NULL)
        strings = n_right;
    else if (n == p->left)
        p->left = n_right;
    else
        p->right = n_right;

    n_right->left = n;
    n->parent = n_right;
}
void rotateRight(struct StringNode *n) /* rotates the string tree to the right */
{
    struct StringNode *n_left = n->left;
    struct StringNode *p = n->parent;

    n->left = n_left->right;

    if (n->left != NULL)
        n->left->parent = n;

    n_left->parent = p;

    if (p == NULL)
        strings = n_left;
    else if (n == p->left)
        p->left = n_left;
    else
        p->right = n_left;

    n_left->right = n;
    n->parent = n_left;
}
struct StringNode *insertString()
{ /* gets new line from stdin and inserts in memory (if it doesn't exist already) and returns a pointer to it */
    struct StringNode *newNode = malloc(sizeof(struct StringNode));
    newNode->color = 1;
    newNode->parent = NULL;
    newNode->left = NULL;
    newNode->right = NULL;
    if (!fgets(newNode->string, sizeof(newNode->string), stdin))
        exit(1);

    struct StringNode *string = NULL;
    strings = treeInsert(strings, newNode, &string);

    if (string == newNode)
        treeFixup(newNode);

    return string;
}
struct StringNode *treeInsert(struct StringNode *root, struct StringNode *n, struct StringNode **s)
{ /* if it doesnt exist, insert n in the strings tree, save pointer to it in s */
    if (root == NULL)
    {
        if (*s == NULL)
            *s = n;
        return n;
    }
    int cmp = strcmp(n->string, root->string);
    if (cmp < 0)
    {
        root->left = treeInsert(root->left, n, s);
        root->left->parent = root;
    }
    else if (cmp > 0)
    {
        root->right = treeInsert(root->right, n, s);
        root->right->parent = root;
    }
    else
    {
        free(n);
        *s = root;
    }
    return root;
}
void treeFixup(struct StringNode *n)
{
    struct StringNode *p = NULL;
    struct StringNode *gp = NULL;

    while ((n != strings) && (n->color != 0) && (n->parent->color == 1))
    {
        p = n->parent;
        gp = p->parent;

        if (p == gp->left)
        { /* parent of n is left child of grand-parent of n */
            struct StringNode *u = gp->right;

            if ((u != NULL) && (u->color == 1))
            { /* if uncle of n is red*/
                gp->color = 1;
                p->color = 0;
                u->color = 0;
                n = gp;
            }
            else
            {
                if (n == p->right)
                {
                    rotateLeft(p);
                    n = p;
                    p = p->parent;
                }
                rotateRight(gp);
                int temp = p->color;
                p->color = gp->color;
                gp->color = temp;
                n = p;
            }
        }
        else
        { /* parent of n is right child of grandparent of n */
            struct StringNode *u = gp->left;

            if ((u != NULL) && (u->color == 1))
            { /* if uncle of n is red*/
                gp->color = 1;
                p->color = 0;
                u->color = 0;
                n = gp;
            }
            else
            {
                if (n == p->left)
                {
                    rotateRight(p);
                    n = p;
                    p = p->parent;
                }
                rotateLeft(gp);
                int temp = p->color;
                p->color = gp->color;
                gp->color = temp;
                n = p;
            }
        }
    }
    strings->color = 0;
}
