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
struct StringNode* strings = NULL;
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
void insertString(char content[MAX_ROW_LENGTH]);
struct StringNode *treeInsert(struct StringNode* n);
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
    int k = 0;
    while (a[k] != '\0')
    {
        if ((a[k] > b[k]) || (b[k] == '\0'))
            return 0;
        ++k;
    }
    return 1;
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
    struct StringNode * n_right = n->right;
    struct StringNode * p = getParent(n);

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
    struct StringNode * n_left = n->left;
    struct StringNode * p = n->parent;

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
void insertString()
{
    struct StringNode newNode;
    newNode.color = 1;
    newNode.parent = NULL;
    newNode.left = NULL;
    newNode.right = NULL;
    fgets(newNode.string, sizeof(newNode.string), stdin);

    strings = treeInsert(&newNode);

    treeFixup(&newNode);
}
struct StringNode *treeInsert(struct StringNode* root, struct StringNode* n)
{
    if (strings == NULL)
        return n;

    if (minStr(n->string, root->string))
    {
        root->left = treeInsert(root->left, n);
        root->left->parent = root;
    }
    else if (maxStr(n->string, root->string))
    {
        root->right = treeInsert(root->right, n);
        root->right->parent = root;
    }

    return root;
}
void treeFixup(struct StringNode *n)
{
}
