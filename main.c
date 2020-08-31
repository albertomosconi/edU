/* edU - Progetto Finale per l'esame di Algoritmi e Principi dell'informatica
 * 2019-2020, Politecnico di Milano
 * Alberto Mosconi */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_ROW_LENGTH 250   /* maximum length of input strings */
#define MAX_CMD_LENGTH 100   /* maximum length of cmd strings */
#define SNAPSHOT_THRESHOLD 5 /* number of commands after which a snapshot of the document is saved */
#define QUIT_C 'q'           /* quit character */
#define CHANGE_C 'c'         /* change character */
#define DELETE_C 'd'         /* delete character */
#define PRINT_C 'p'          /* print character */
#define UNDO_C 'u'           /* undo character */
#define REDO_C 'r'           /* redo character */
// #define DEBUG                /* if un-commented activates "debug mode" (lots of printfs) */

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
    int *mod_index;            /* list of modified indexes for saved lines */
    struct Command *prev;      /* previous command in the stack*/
    struct Command *next;      /* next command in the stack */
    char c;                    /* command character */
    int i1, i2, mi1, mi2;      /* original indexes and modified indexes */
};
struct Snapshot /* state of the document */
{
    struct Snapshot *prev;        /* previous snapshot */
    struct Snapshot *next;        /* next snapshot */
    struct StringNode **document; /* array of the document strings */
    int length;                   /* total number of lines */
};
/* GENERAL VARIABLES */
char raw_cmd[MAX_CMD_LENGTH];          /* command buffer */
char c;                                /* command character [q, c, d, p, u, r] */
int i1, i2;                            /* line indexes specified in the command */
int *mods = NULL;                      /* array of modifiers */
int modlen = 1;                        /* stores the line number after which all mod values are the same */
int apply = 0;                         /* if 1 applies consecutive undos and redos at once */
int undoBuffer = 0;                    /* when positive -> how many undos to apply, when negative -> redos */
int totCommands = 0, currCommands = 0; /* tot number of commands executed and current number of commands */
int documentLength = 0;                /* total number of lines in the document */
int savingState = 0;
struct StringNode *strings = NULL;      /* red black tree containing all the strings */
struct Command *latest_command = NULL;  /* pointer to latest command */
struct Command *current_command = NULL; /* pointer to current command */
struct Snapshot *latest_state = NULL;   /* pointer to latest document state snapshot */
struct Snapshot *current_state = NULL;  /* pointer to current document state snapshot */
/* MAIN FUNCTIONS */
void getInput();     /* receive and parse input commands from stdin */
void handleChange(); /* process change command */
void handleDelete(); /* procerss delete command */
void handlePrint();  /* process print command */
void handleUndo();   /* process undo command */
void handleRedo();   /* process redo command */
/* HELPER FUNCTIONS */
int max(int a, int b);                                     /* returns maximum between a and b */
int min(int a, int b);                                     /* returns minimum between a and b */
void findLines(int a, int b, struct StringNode ***buffer); /* returns array of lines between two indexes */
void documentInit();                                       /* initialize document */
void cmdToHistory();                                       /* adds a new change or delete command to the history */
void saveDocumentState();                                  /* save a copy of the document state */
void applyUndos();                                         /* trigger application of grouped undos and redos */
/* STRINGS TREE FUNCTIONS */
struct StringNode *insertString(); /* gets new line from stdin and inserts in memory (if it doesn't exist already) and returns a pointer to it */
struct StringNode *treeInsert(
    struct StringNode *root, struct StringNode *n, struct StringNode **s); /* if it doesnt exist, insert n in the strings tree, save pointer to it in s */
void rotateLeft(struct StringNode *n);                                     /* rotates the string tree to the left */
void rotateRight(struct StringNode *n);                                    /* rotates the string tree to the right */
void treeFixup(struct StringNode *n);                                      /* fix colors of tree */

int main()
{
    documentInit();
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
    if (!fgets(raw_cmd, sizeof(raw_cmd), stdin))
        exit(1); /* if fail stop program */

    int success = sscanf(raw_cmd, "%d,%d%c", &i1, &i2, &c);
    if (1 == success)
    { /* with only one digits it's either undo or redo */
        sscanf(raw_cmd, "%d%c", &i1, &c);
        i2 = -1;
    }
    else if (0 == success)
    { /* only one character, it's 'q' or '.' */
        sscanf(raw_cmd, "%c", &c);
        i1 = -1;
        i2 = -1;
    }
    if ((c != 'u') && (c != 'r') && (c != '.'))
        applyUndos();
}
void handleChange() /* process change command */
{
    cmdToHistory(); /* add a new command struct to the list */
#ifdef DEBUG
    printf("CHANGE %d TO %d\n", i1, i2);
#endif
    latest_command->lines = malloc((i2 - i1 + 1) * sizeof(struct StringNode *));
    latest_command->mod_index = malloc((i2 - i1 + 1) * sizeof(int));
    for (int k = 0; k < i2 - i1 + 1; ++k)
    { /* insert the new content */
        latest_command->lines[k] = insertString();
        latest_command->mod_index[k] = i1 + k + mods[min(i1 + k, modlen) - 1];
#ifdef DEBUG
        printf("\nMOD INDEX %d\n\n", latest_command->mod_index[k]);
        printf("|\t%d: %s", i1 + k + mods[min(i1 + k, modlen) - 1], latest_command->lines[k]->string);
#endif
    }
    latest_command->next = NULL;
    current_command = latest_command;

    /* update document length */
    if (i2 > documentLength)
        documentLength = i2;

    /* threshold is reached, create new state snapshot */
    if (currCommands % SNAPSHOT_THRESHOLD == 0)
        saveDocumentState();
#ifdef DEBUG
    printf("|_________________________________________\n");
#endif
}
void handleDelete() /* process delete command */
{
    cmdToHistory(); /* add a new command struct to the list */

    if (i1 >= modlen)
    { /* if the first deleted line is not in mods, make it bigger and copy old values */
        int *temp = calloc(i1, sizeof(*temp));

        for (int k = 0; k < i1; ++k)
            temp[k] = mods[min(k, modlen - 1)];

        modlen = i1;
        free(mods);
        mods = temp;
    }
    for (int k = i1 - 1; k < modlen; ++k)
        /* update new values */
        mods[k] += i2 - i1 + 1;

    /* update document length */
    if (i2 > documentLength)
        documentLength -= (i2 - i1 + 1 - i2 + documentLength);
    else
        documentLength -= i2 - i1 + 1;

    /* threshold is reached, create new state snapshot */
    if (currCommands % SNAPSHOT_THRESHOLD == 0)
        saveDocumentState();
#ifdef DEBUG
    printf("DELETE %d TO %d\n|\tmods: ", i1, i2);
    for (int k = 0; k < modlen; ++k)
        printf("%d ", mods[k]);
    printf("\n|_________________________________________\n");
#endif
}
void handlePrint() /* process print command */
{
#ifdef DEBUG
    printf("PRINTING LINES %d TO %d\n", i1, i2);
#endif
    int to_find = i2 - i1 + 1;
    struct StringNode **buffer = malloc(to_find * sizeof(*buffer));

    findLines(i1, i2, &buffer);

    /* print the buffer */
    for (int k = 0; k < i2 - i1 + 1; ++k)
        if (!buffer[k])
            fputs(".\n", stdout);
        else
            fputs(buffer[k]->string, stdout);
    /* don't need this anymore */
    free(buffer);
#ifdef DEBUG
    printf("|_________________________________________\n");
#endif
}
void handleUndo() /* process undo command */
{
    if (apply != 0)
    { /* apply grouped undos */
        while ((current_command != NULL) && (current_command->prev != NULL) && (undoBuffer > 0))
        {
            if (current_command->c == 'd')
            { /* if undoing a delete command, adjust index modifiers */
                for (int k = current_command->i1 - 1; k < modlen; ++k)
                {
                    mods[k] -= current_command->i2 - current_command->i1 + 1;
                }
            }
            if ((current_state != NULL) && (currCommands != totCommands) && ((currCommands + 1) % SNAPSHOT_THRESHOLD == 0))
                current_state = current_state->prev;

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
        {
            current_command = current_command->next;
            if (current_command->c == 'd')
            { /* if redoing a delete command, adjust index modifiers */
                for (int k = current_command->i1 - 1; k < modlen; ++k)
                {
                    mods[k] += current_command->i2 - current_command->i1 + 1;
                }
            }
            if ((current_state != latest_state) && (currCommands != totCommands) && (currCommands % SNAPSHOT_THRESHOLD == 0))
                current_state = current_state->next;

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
void findLines(int a, int b, struct StringNode ***buffer) /* returns array of lines between two indexes */
{
    int to_find = b - a + 1;
    int *found = calloc(to_find, sizeof(*found));
    int nodesVisited = -1; /* number of previously visited nodes */

    for (struct Command *curr_cmd = current_command; (curr_cmd->prev != NULL) && (to_find > 0); curr_cmd = curr_cmd->prev)
    {
#ifdef DEBUG
        printf(" - C: %c\tI1: %d\tI2: %d\n", curr_cmd->c, curr_cmd->i1, curr_cmd->i2);
#endif
        ++nodesVisited;
        if ((currCommands - nodesVisited) % SNAPSHOT_THRESHOLD == 0)
        {
            if (!savingState)
            {
                for (int k = 0; (k < b - a + 1) && (a + k - 1 < current_state->length); ++k)
                {
                    if (!found[k])
                    {
                        found[k] = 1;
                        (*buffer)[k] = current_state->document[a + k - 1];
                    }
                }
                break;
            }
            else
                savingState = 0;
        }
        if (curr_cmd->c == 'c')
        {
#ifdef DEBUG
            printf("|\tneeded lines: ");
            for (int j = 0; j < b - a + 1; ++j)
                printf("%d ", a + j + mods[min(a + j, modlen) - 1]);
            printf("\n|\tavailable %d lines: ", curr_cmd->i2 - curr_cmd->i1 + 1);
            for (int j = 0; j < curr_cmd->i2 - curr_cmd->i1 + 1; ++j)
                printf("%d ", curr_cmd->mod_index[j]);
            printf("\n|\tfound: ");
            for (int j = 0; j < b - a + 1; ++j)
                printf("%d ", found[j]);
            printf("\n");
            printf("check %d <= %d %d\n", a + mods[min(a, modlen) - 1], curr_cmd->mod_index[curr_cmd->i2 - curr_cmd->i1], curr_cmd->i2);
#endif
            if ((a + mods[min(a, modlen) - 1] <= curr_cmd->mod_index[curr_cmd->i2 - curr_cmd->i1]) && (b + mods[min(b, modlen) - 1] >= curr_cmd->mod_index[0]))
            {
                for (int j = 0; (j < b - a + 1) && (to_find > 0); ++j)
                {
#ifdef DEBUG
                    printf(" - %d %d\n", a + j + mods[min(a + j, modlen) - 1], curr_cmd->mod_index[j]);
#endif
                    for (int k = 0; (k < curr_cmd->i2 - curr_cmd->i1 + 1) && (to_find > 0); ++k)
                    {
                        if ((j < b - a + 1) && (found[j] == 0) && (a + j + mods[min(a + j, modlen) - 1] == curr_cmd->mod_index[k]))
                        {
#ifdef DEBUG
                            printf(" + FOUND %d == %d\n", a + j + mods[min(a + j, modlen) - 1], curr_cmd->mod_index[k]);
#endif
                            found[j] = 1;
                            (*buffer)[j] = curr_cmd->lines[k];
                            ++j;
                            --to_find;
                        }
                    }
                }
            }
        }
    }
    for (int k = 0; k < i2 - i1 + 1; ++k)
        if (!found[k])
            (*buffer)[k] = NULL;
}
void documentInit() /* initialize document */
{
#ifdef DEBUG
    printf("Initializing modifiers...\n");
#endif
    /*  initialize index modifiers array */
    mods = calloc(1, sizeof(int));
#ifdef DEBUG
    printf("Initializing command history...\n");
#endif
    /* initialize command history */
    latest_command = malloc(sizeof(*latest_command));
    latest_command->c = '#';
    latest_command->i1 = -1;
    latest_command->i2 = -1;
    latest_command->prev = NULL;
    current_command = latest_command;
#ifdef DEBUG
    printf("Initializing state history...\n");
#endif
    /* initialize document state history */
    latest_state = malloc(sizeof(*latest_state));
    latest_state->length = -1;
    latest_state->prev = NULL;
    current_state = latest_state;
}
void cmdToHistory() /* adds a new change or delete command to the history */
{
    /* delete undone commands, start new branch */
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
    current_command = latest_command;
    ++currCommands;
    totCommands = currCommands;
}
void saveDocumentState() /* save a copy of the document state */
{
#ifdef DEBUG
    printf("THRESHOLD REACHED\nsaving document state...\n");
#endif
    latest_state->next = malloc(sizeof(*latest_state));
    latest_state->next->prev = current_state;
    latest_state = latest_state->next;
    latest_state->length = documentLength;
    latest_state->document = malloc(documentLength * sizeof(*(latest_state->document)));
    current_state = latest_state;

    savingState = 1;
    findLines(1, documentLength, &(latest_state->document));

#ifdef DEBUG
    /* print the buffer */
    for (int k = 0; k < documentLength; ++k)
        if (!latest_state->document[k])
            fputs("|\t.\n", stdout);
        else
        {
            fputs("|\t", stdout);
            fputs(latest_state->document[k]->string, stdout);
        }
#endif
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
