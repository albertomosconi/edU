/* edU - Progetto Finale per l'esame di Algoritmi e Principi dell'informatica
 * 2019-2020, Politecnico di Milano
 * Alberto Mosconi */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_ROW_LENGTH 250 /* maximum length of input strings */
#define MAX_CMD_LENGTH 100 /* maximum length of cmd strings */
#define QUIT_C 'q'
#define CHANGE_C 'c'
#define DELETE_C 'd'
#define PRINT_C 'p'
#define UNDO_C 'u'
#define REDO_C 'r'
/* STRUCTURES */
struct StringNode /* node in the strings tree */
{
    struct StringNode *parent;   /* parent node */
    struct StringNode *left;     /* left node */
    struct StringNode *right;    /* right node */
    int color;                   /* node color, 1->red, 0->black */
    char string[MAX_ROW_LENGTH]; /* string contents */
};
struct Command /* command object to be stored in the history stack */
{
    struct StringNode **lines; /* list of pointers to the lines in the tree */
    struct Command *prev;      /* previous command in the stack*/
    struct Command *next;      /* next command in the stack */
    char c;                    /* command character */
    int i1, i2, mi1, mi2;      /* original indexes and modified indexes */
};
/* GENERAL VARIABLES */
char raw_cmd[MAX_CMD_LENGTH];           /* command buffer */
char c;                                 /* command character [q, c, d, p, u, r] */
int i1, i2;                             /* line indexes specified in the command */
int *mods = NULL;                       /* array of modifiers */
int modlen = 1;                         /* stores the line number after which all mod values are the same */
int apply = 0;                          /* if 1 applies consecutive undos and redos at once */
int undoBuffer = 0;                     /* when positive -> how many undos to apply, when negative -> redos */
int totCommands = 0, currCommands = 0;  /* tot number of commands executed and current number of commands */
struct StringNode *strings = NULL;      /* red black tree containing all the strings */
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
int max(int a, int b); /* returns maximum between a and b */
int min(int a, int b); /* returns minimum between a and b */
void cmdToHistory();   /* adds a new change or delete command to the history */
void applyUndos();     /* trigger application of grouped undos and redos */
/* STRINGS TREE FUNCTIONS */
struct StringNode *insertString();
struct StringNode *treeInsert(struct StringNode *root, struct StringNode *n, struct StringNode **s);
void rotateLeft(struct StringNode *n);  /* rotates the string tree to the left */
void rotateRight(struct StringNode *n); /* rotates the string tree to the right */
void treeFixup(struct StringNode *n);   /* fix colors of tree */

int main()
{
    mods = calloc(1, sizeof(int));
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
    if ((c != 'u') && (c != 'r'))
        applyUndos();
}
void handleChange()
{ /* process change command */

    cmdToHistory(); /* add a new command struct to the list */
    latest_command->lines = malloc((i2 - i1 + 1) * sizeof(struct StringNode *));
    for (int k = 0; k < i2 - i1 + 1; ++k)
    { /* insert the new content */
        latest_command->lines[k] = insertString();
    }
    latest_command->next = NULL;
    current_command = latest_command;
}
void handleDelete() /* process delete command */
{
    cmdToHistory(); /* add a new command struct to the list */

    if (i1 + current_command->mi1 > modlen)
    {
        int *temp = calloc(i1, sizeof(int));

        for (int k = 0; k < i1; ++k)
            temp[k] = mods[min(k, modlen - 1)];

        modlen = i1;
        free(mods);
        mods = temp;
    }
    for (int k = i1 - 1; k < modlen; ++k)
        mods[k] += i2 - i1 + 1;
}
void handlePrint() /* process print command */
{
    struct StringNode **buffer = malloc((i2 - i1 + 1) * sizeof(struct StringNode *));
    int *found = (int *)calloc((i2 - i1 + 1), sizeof(int));
    int to_find = i2 - i1 + 1; /* how many elements are still to be found */
    int len = to_find;

    for (struct Command *curr_cmd = current_command; (curr_cmd != NULL) && (to_find > 0); curr_cmd = curr_cmd->prev)
    {
        if (curr_cmd->c == 'c')
        {
            if ((i1 + mods[min(i1 - 1, modlen - 1)] <= curr_cmd->i2 + curr_cmd->mi2) && (i2 + mods[min(i2 - 1, modlen - 1)] >= curr_cmd->i1 + curr_cmd->mi1))
            {
                for (int j = 0; j < i2 + mods[min(i2 - 1, modlen - 1)] - i1 - mods[min(i1 - 1, modlen - 1)] + 1; ++j)
                {
                    if ((i1 + j + mods[min(i1 + j - 1, modlen - 1)] <= curr_cmd->i2 + curr_cmd->mi2) && (i1 + j + mods[min(i1 + j - 1, modlen - 1)] >= curr_cmd->i1 + curr_cmd->mi1) && (!found[j]))
                    {
                        found[j] = 1;
                        buffer[j] = curr_cmd->lines[i1 + j + mods[min(i1 + j - 1, modlen - 1)] - curr_cmd->i1 - curr_cmd->mi1];
                        --to_find;
                    }
                }
            }
        }
    }
    /* print the buffer */
    for (int k = 0; k < len; ++k)
    {
        if (!found[k])
            fputs(".\n", stdout);
        else
            fputs(buffer[k]->string, stdout);
    }
    free(buffer);
    free(found);
}
void handleUndo() /* process undo command */
{
    if (apply != 0)
    {
        while ((current_command != NULL) && (current_command->prev != NULL) && (undoBuffer > 0))
        {
            if (current_command->c == 'd')
            {
                for (int k = current_command->i1 - 1; k < modlen; ++k)
                {
                    mods[k] -= current_command->i2 - current_command->i1 + 1;
                }
            }
            current_command = current_command->prev;
            --undoBuffer;
            --currCommands;
        }
    }
    else if (currCommands - undoBuffer - i1 >= 0)
        undoBuffer += i1;
    else if (currCommands - undoBuffer - i1 < 0)
        undoBuffer = currCommands;
}
void handleRedo() /* process redo command */
{
    if (apply != 0)
    {
        undoBuffer *= -1;
        while ((current_command != NULL) && (current_command->next != NULL) && (undoBuffer > 0))
        {
            current_command = current_command->next;
            if (current_command->c == 'd')
            {
                for (int k = current_command->i1 - 1; k < modlen; ++k)
                {
                    mods[k] += current_command->i2 - current_command->i1 + 1;
                }
            }
            --undoBuffer;
            ++currCommands;
        }
    }
    else if (currCommands - undoBuffer + i1 <= totCommands)
        undoBuffer -= i1;
    else if (currCommands - undoBuffer + i1 > totCommands)
        undoBuffer = currCommands - totCommands;
}
void cmdToHistory() /* adds a new change or delete command to the history */
{
    while (latest_command != current_command)
    {
        latest_command = latest_command->prev;
        free(latest_command->next);
    }
    if (!latest_command)
    {
        latest_command = malloc(sizeof(struct Command));
        latest_command->prev = NULL;
    }
    latest_command->next = malloc(sizeof(struct Command));
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
    fgets(newNode->string, sizeof(newNode->string), stdin);

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
