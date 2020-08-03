#include <stdio.h>
#include <stdlib.h>
#define MAX_ROW_LENGTH 1025 /* maximum length of input strings */
/* STRUCTURES */
struct StringNode /* node in the strings tree */
{
    struct StringNode *parent;   /* parent node */
    struct StringNode *left;     /* left node */
    struct StringNode *right;    /* right node */
    int color;                   /* node color, 1->red, 0->black */
    char string[MAX_ROW_LENGTH]; /* string contents */
};
struct Command
{ /* command object to be stored in the history stack */
    char c;
    int i1, i2;
    struct StringNode **lines; /* list of pointers to the lines in the tree */
    struct Command *prev;      /* previous command in the stack*/
    struct Command *next;      /* next command in the stack */
};
struct Modifier
{
    int lim;               /* index above which the modifier must be applied */
    int val;               /* number of deleted lines */
    struct Modifier *next; /* next modifier */
    struct Modifier *prev; /* previous modifier */
};
/* GENERAL VARIABLES */
char raw_cmd[MAX_ROW_LENGTH]; /* command buffer */
int i1, i2;                   /* line indexes specified in the command */
char c;                       /* command character [q, c, d, p, u, r] */
struct StringNode *strings = NULL;
struct Command *command_history = NULL;
struct Command *actual_command = NULL;
struct Modifier *mods = NULL;
struct Modifier *actual_mod = NULL;
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
int minStr(char *a, char *b); /* returns 1 if the string 'a' is < than 'b', otherwise 0 */
int maxStr(char *a, char *b); /* returns 1 if the string 'a' is > than 'b', otherwise 0 */
void updateIndexes();         /* applies all the modifiers to the indexes */
void cmdToHistory();          /* adds a new change or delete command to the history */
/* STRINGS TREE FUNCTIONS */
struct StringNode *getParent(struct StringNode *n);  /* returns the parent of a given StringNode */
struct StringNode *getGrandpa(struct StringNode *n); /* returns the parent of the parent of the given StringNode */
struct StringNode *getSibling(struct StringNode *n); /* returns the sibling of a given StringNode */
struct StringNode *getUncle(struct StringNode *n);   /* return the sibling of the parent of a given StringNode */
void rotateLeft(struct StringNode *n);               /* rotates the string tree to the left */
void rotateRight(struct StringNode *n);              /* rotates the string tree to the right */
struct StringNode *insertString();
struct StringNode *treeInsert(struct StringNode *root, struct StringNode *n);
void treeFixup(struct StringNode *n);

int main()
{
    while (1)
    {
        getInput(); /* fetch and parse input */
        switch (c)  /* handle different commands */
        {
        case 'q': /* terminate the program */
            return 0;
        case 'c': /* change lines */
            handleChange();
            break;
        case 'd': /* delete lines */
            handleDelete();
            break;
        case 'p': /* print lines */
            handlePrint();
            break;
        case 'u': /* undo commands */
            handleUndo();
            break;
        case 'r': /* redo commands */
            handleRedo();
            break;
        case '.': /* termination character */
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
}
void handleChange()
{ /* process change command */
    while (command_history != actual_command)
    {
        command_history = command_history->prev;
        free(command_history->next);
    }

    updateIndexes(); /* update the indexes with the modifiers */

    if (!command_history)
    {
        command_history = malloc(sizeof(struct Command));
        command_history->prev = NULL;
    }
    else
    {
        command_history->next = malloc(sizeof(struct Command));
        command_history->next->prev = command_history;
        command_history = command_history->next;
    }
    command_history->c = c;
    command_history->i1 = i1;
    command_history->i2 = i2;
    command_history->lines = malloc((i2 - i1 + 1) * sizeof(struct StringNode *));
    for (int k = 0; k < i2 - i1 + 1; ++k)
    {
        command_history->lines[k] = insertString();
    }
    command_history->next = NULL;
    actual_command = command_history;
}
void handleDelete()
{ /* procerss delete command */
}
void handlePrint()
{ /* process print command */
    struct StringNode **buffer = malloc((i2 - i1 + 1) * sizeof(struct StringNode *));
    int *found = calloc((i2 - i1 + 1), sizeof(int));
    int to_find = i2 - i1 + 1; /* how many elements are still to be found */
    int len = to_find;

    updateIndexes(); /* update the indexes with the modifiers */

    struct Command *curr_cmd = actual_command;
    while ((curr_cmd != NULL) && (to_find > 0))
    {
        if ((i1 <= curr_cmd->i2) && (i2 >= curr_cmd->i1))
        {
            /* save lines from max(i1, curr_cmd->i1) to min(i2, curr_cmd->i2) */
            for (int k = max(i1 - curr_cmd->i1, 0); k < min(i2, curr_cmd->i2) - curr_cmd->i1 + 1; ++k)
            {
                if (!found[k + (curr_cmd->i1 - i1)])
                {
                    buffer[k + (curr_cmd->i1 - i1)] = curr_cmd->lines[k];
                    found[k + (curr_cmd->i1 - i1)] = 1;
                    --to_find;
                }
            }
        }
        curr_cmd = curr_cmd->prev;
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
void handleUndo()
{ /* process undo command */
}
void handleRedo()
{ /* process redo command */
}

void updateIndexes()
{ /* applies all the modifiers to the indexes */
}
void cmdToHistory()
{ /* adds a new change or delete command to the history */
}
int max(int a, int b)
{ /* returns maximum between a and b */
    if (a >= b)
        return a;
    return b;
}
int min(int a, int b)
{ /* returns minimum between a and b */
    if (a > b)
        return b;
    return a;
}
int minStr(char *a, char *b)
{ /* returns 1 if the string 'a' is < than 'b', otherwise 0 */
    int k = 0;

    while (a[k] != '\0')
    {
        if ((a[k] > b[k]) || (b[k] == '\0'))
            return 0;
        ++k;
    }
    return 1;
}
int strCmp(char *a, char *b)
{ /* returns 0 if a == b, >0 if a>b, <0 if a<b */
    char *s1 = (char *)a;
    char *s2 = (char *)b;
    char c1, c2;
    do
    {
        c1 = (char)*s1++;
        c2 = (char)*s2++;
        if (c1 == '\0')
            return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
}
struct StringNode *getParent(struct StringNode *n) /* returns the parent of a given StringNode */
{
    if (NULL == n)
        return NULL;
    return n->parent;
}
struct StringNode *getGrandpa(struct StringNode *n) /* returns the parent of the parent of the given StringNode */
{
    return getParent(getParent(n));
}
struct StringNode *getSibling(struct StringNode *n) /* returns the sibling of a given StringNode */
{
    struct StringNode *p = getParent(n);
    if (NULL == p)
        return NULL;
    if (n == p->left)
        return p->right;
    else
        return p->left;
}
struct StringNode *getUncle(struct StringNode *n) /* return the sibling of the parent of a given StringNode */
{
    struct StringNode *p = getParent(n);
    return getSibling(p);
}
void rotateLeft(struct StringNode *n) /* rotates the string tree to the left */
{
    struct StringNode *n_right = n->right;
    struct StringNode *p = getParent(n);

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

    if (n->right != NULL)
        n->left->parent = n;

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
{
    struct StringNode *newNode = malloc(sizeof(struct StringNode));
    newNode->color = 1;
    newNode->parent = NULL;
    newNode->left = NULL;
    newNode->right = NULL;
    fgets(newNode->string, sizeof(newNode->string), stdin);

    strings = treeInsert(strings, newNode);

    treeFixup(newNode);

    return newNode;
}
struct StringNode *treeInsert(struct StringNode *root, struct StringNode *n)
{
    if (root == NULL)
        return n;

    if (strCmp(n->string, root->string) < 0)
    {
        root->left = treeInsert(root->left, n);
        root->left->parent = root;
    }
    else if (strCmp(n->string, root->string) > 0)
    {
        root->right = treeInsert(root->right, n);
        root->right->parent = root;
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
