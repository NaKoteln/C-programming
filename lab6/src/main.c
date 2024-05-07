#include <stdio.h>
#include <malloc.h>

typedef int MyData;
typedef struct TTree {
    struct TTree *right;
    struct TTree *left;
    MyData data;
    int height;
} TreeAVL;

void *MyMalloc(const TreeAVL *mem, TreeAVL **current, const unsigned int MemSize) {
    if ((*current - mem) / sizeof(TreeAVL) < MemSize) {
        return (*current)++;
    }
    return NULL;
}

TreeAVL *CreateNode(const MyData *data, const TreeAVL *mem, TreeAVL **current, const unsigned int MemSize) {
    TreeAVL *temporary = MyMalloc(mem, current, MemSize);
    temporary->left = NULL;
    temporary->right = NULL;
    temporary->data = *data;
    temporary->height = 1;
    return temporary;
}

int GetHeight(const TreeAVL *tree) {
    return (tree != NULL) ? tree->height : 0;
}

void UpdateHeight(TreeAVL *tree) {
    tree->height = (((GetHeight(tree->left)) > (GetHeight(tree->right))) ? (GetHeight(tree->left)) :
                    (GetHeight(tree->right))) + 1;
}

void LittleLeftRotation(TreeAVL **tree) {
    TreeAVL *a = *tree;
    TreeAVL *b = a->right;
    TreeAVL *c = b->left;
    a->right = c;
    b->left = a;
    *tree = b;
    UpdateHeight(a);
    UpdateHeight(b);
}

void LittleRightRotation(TreeAVL **tree) {
    TreeAVL *a = *tree;
    TreeAVL *b = a->left;
    TreeAVL *c = b->right;
    a->left = c;
    b->right = a;
    *tree = b;
    UpdateHeight(a);
    UpdateHeight(b);
}

void BigLeftRotation(TreeAVL **tree) {
    LittleRightRotation(&((*tree)->right));
    LittleLeftRotation(tree);
}

void BigRightRotation(TreeAVL **tree) {
    LittleLeftRotation(&((*tree)->left));
    LittleRightRotation(tree);
}

void BalanceTree(TreeAVL **tree) {
    if ((*tree)->right != NULL && GetHeight((*tree)->right) - GetHeight((*tree)->left) == 2) {
        if (GetHeight((*tree)->right->left) <= GetHeight((*tree)->right->right)) {
            LittleLeftRotation(tree);
        } else {
            BigLeftRotation(tree);
        }
    } else if ((*tree)->left != NULL && GetHeight((*tree)->left) - GetHeight((*tree)->right) == 2) {
        if (GetHeight((*tree)->left->right) <= GetHeight((*tree)->left->left)) {
            LittleRightRotation(tree);
        } else {
            BigRightRotation(tree);
        }
    }
}

void
AddValueToTree(TreeAVL **tree, const MyData value, const TreeAVL *mem, TreeAVL **current, const unsigned int MemSize) {
    if (*tree == NULL) {
        *tree = CreateNode(&value, mem, current, MemSize);
        return;
    }
    if (value < (*tree)->data) {
        AddValueToTree(&(*tree)->left, value, mem, current, MemSize);
    } else {
        AddValueToTree(&(*tree)->right, value, mem, current, MemSize);
    }
    BalanceTree(tree);
    UpdateHeight(*tree);
}

int main() {
    int n;
    if (fscanf(stdin, "%d", &n) != 1) {
        return 1;
    }
    TreeAVL *avl_tree = NULL;
    TreeAVL *mem = calloc(n, sizeof(TreeAVL));
    TreeAVL *current = mem;
    for (int i = 0; i < n; ++i) {
        int num;
        if (fscanf(stdin, "%d", &num) != 1) {
            free(mem);
            return 1;
        }
        AddValueToTree(&avl_tree, num, mem, &current, n);
    }
    fprintf(stdout, "%d", GetHeight(avl_tree));
    free(mem);
    return 0;
}
