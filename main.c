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
/* GENERAL VARIABLES */
char raw_cmd[MAX_ROW_LENGTH]; /* command buffer */
int i1, i2;                   /* line indexes specified in the command */
char c;                       /* command character [q, c, d, p, u, r] */
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
void updateIndexes();         /* applies all the modifiers to the indexes */
void cmdToHistory();          /* adds a new change or delete command to the history */
/* STRINGS TREE FUNCTIONS */
struct StringNode *getParent(struct StringNode *n);  /* returns the parent of a given StringNode */
struct StringNode *getGrandpa(struct StringNode *n); /* returns the parent of the parent of the given StringNode */
struct StringNode *getSibling(struct StringNode *n); /* returns the sibling of a given StringNode */
struct StringNode *getUncle(struct StringNode *n);   /* return the sibling of the parent of a given StringNode */
void rotateLeft(struct StringNode *n);               /* rotates the string tree to the left */
void rotateRight(struct StringNode *n);              /* rotates the string tree to the right */

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
}
void handleDelete()
{ /* procerss delete command */
}
void handlePrint()
{ /* process print command */
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
}
void rotateRight(struct StringNode *n) /* rotates the string tree to the right */
{
}