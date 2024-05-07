#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMPRESSION 'c'
#define DECOMPRESSION 'd'

typedef struct _file {
    FILE *in;
    FILE *out;
} Files;

typedef struct _buff {
    int len;
    unsigned char buffer;
} Buff;

typedef struct _Tree {
    int priority;
    char buff[257];
    unsigned char value;
    struct _Tree *right;
    struct _Tree *left;
} Tree;

typedef struct _ListNode {
    int priority;
    Tree *value;
    struct _ListNode *next;
} ListNode;

typedef struct _priQueue {
    ListNode *head;
    ListNode *tail;
} MinPriQueue;

Tree *CreateTree() {
    Tree *tree = calloc(1, sizeof(Tree));
    return tree;
}

void DeleteTree(Tree *tree) {
    if (tree) {
        if (tree->left) {
            DeleteTree(tree->left);
        }
        if (tree->right) {
            DeleteTree(tree->right);
        }
        free(tree);
    }
}

MinPriQueue *CreateQueue() {
    MinPriQueue *queue = calloc(1, sizeof(MinPriQueue));
    return queue;
}

void Push(MinPriQueue *queue, Tree *elem) {
    ListNode *temp = calloc(1, sizeof(ListNode));
    ListNode *temp2;
    temp->value = elem;
    temp->priority = temp->value->priority;
    if ((queue->head == NULL) && (queue->tail == NULL)) {
        queue->tail = temp;
        queue->head = temp;
    } else {
        temp2 = queue->head;
        if (temp->priority < temp2->priority) {
            temp->next = temp2;
            queue->head = temp;
        } else {
            while ((temp2->next) && (temp->priority > temp2->next->priority)) {
                temp2 = temp2->next;
            }
            if (!temp2->next) {
                temp2->next = temp;
                queue->tail = temp;
            } else {
                temp->next = temp2->next;
                temp2->next = temp;
            }
        }
    }
}

int IsEmpty(MinPriQueue *queue) {
    return (queue->head) == NULL;
}

Tree *Pop(MinPriQueue *queue) {
    Tree *tree = queue->head->value;
    ListNode *oldHead = queue->head;
    queue->head = queue->head->next;
    free(oldHead);
    if (IsEmpty(queue)) {
        queue->tail = NULL;
    }
    return tree;
}

void DeleteQueue(MinPriQueue *queue) {
    while (!IsEmpty(queue)) {
        free(Pop(queue));
    }
    free(queue);
}

Files *OpenFiles() {
    Files *f;
    f = calloc(1, sizeof(Files));
    f->in = fopen("in.txt", "rb");
    f->out = fopen("out.txt", "wb");
    return f;
}

void CloseFiles(Files *f) {
    fclose(f->in);
    fclose(f->out);
    free(f);
}

Tree *CopyTree(Tree* tree) {
    Tree * newTree = calloc(1, sizeof(Tree));
    memcpy(newTree, tree, sizeof(Tree));
    return newTree;
}

Tree *CreateHuffmanTree(Files *f, size_t *countLen) {
    Tree *alphabet;
    alphabet = calloc(256, sizeof(Tree));
    int c;
    while ((c = fgetc(f->in)) != EOF) {
        (*countLen)++;
        alphabet[c].priority++;
        alphabet[c].value = c;
    }
    if (!(*countLen)) {
        free(alphabet);
        return NULL;
    }
    MinPriQueue *queue;
    queue = CreateQueue();
    for (int i = 0; i < 256; i++) {
        if (alphabet[i].priority) {
            Push(queue, CopyTree(&alphabet[i]));
        }
    }
    while (queue->tail != queue->head) {
        Tree *temp1 = Pop(queue);
        Tree *temp2 = Pop(queue);
        Tree *temp3 = calloc(1, sizeof(Tree));
        temp3->left = temp1;
        temp3->right = temp2;
        temp3->priority = temp1->priority + temp2->priority;
        Push(queue, temp3);
    }
    Tree *tree = Pop(queue);
    DeleteQueue(queue);
    free(alphabet);
    return tree;
}

void WriteBit(Files *f, Buff *b, unsigned char *insert);

void WriteByte(Files *f, Buff *b, const unsigned char *insert);

void TreeTraversal(Files *f, Tree *tree, char **code_table, Buff *b) {
    if (!tree)
        return;
    size_t len = strlen(tree->buff);
    if (tree->left || tree->right) {
        WriteBit(f, b, (unsigned char *) "0");
        if (tree->left) {
            tree->buff[len] = '0';
            strcpy(tree->left->buff, tree->buff);
            TreeTraversal(f, tree->left, code_table, b);
        }
        if (tree->right) {
            tree->buff[len] = '1';
            strcpy(tree->right->buff, tree->buff);
            TreeTraversal(f, tree->right, code_table, b);
        }
    } else {
        WriteBit(f, b, (unsigned char *) "1");
        strcpy(code_table[tree->value], tree->buff);
        WriteByte(f, b, &tree->value);
    }
}

char **CreateCodeTable(Files *f, Tree *tree, Buff *b) {
    char **codeTable;
    codeTable = calloc(256, sizeof(char *));
    for (int i = 0; i < 256; i++)
        codeTable[i] = calloc((256), sizeof(char));
    TreeTraversal(f, tree, codeTable, b);
    return codeTable;
}

void DeleteCodeTable(char **codeTable) {
    for (int i = 0; i < 256; i++) {
        free(codeTable[i]);
    }
    free(codeTable);
}

void WriteBit(Files *f, Buff *b, unsigned char *insert) {
    size_t len = strlen((char *) insert);
    for (int i = 0; len > 0; i++) {
        if (b->len == 8) {
            fwrite(&b->buffer, 1, 1, f->out); // 1 байт = 8 бит
            b->buffer = 0;
            b->len = 0;
        }

        unsigned char cleanMask = ~((0177) >> b->len);
        unsigned char setMask = (insert[i] - '0') << (7 - b->len);

        b->len++;
        b->buffer &= cleanMask;
        b->buffer |= setMask;
        len--;
    }
}

void WriteByte(Files *f, Buff *b, const unsigned char *insert) {
    unsigned char cleanMask = ~((0177) >> b->len);
    unsigned char setMask = (*insert >> b->len);

    unsigned char temp = b->buffer & cleanMask;
    temp |= setMask;

    b->buffer = (*insert << (8 - b->len));
    fwrite(&temp, 1, 1, f->out);
}

void WriteC(Files *f, char **codeTable, Buff *b) {
    int c;
    while ((c = fgetc(f->in)) != EOF) {
        WriteBit(f, b, (unsigned char *) codeTable[c]);
    }
    if (b->len) {
        fwrite(&b->buffer, 1, 1, f->out);
    }
    b->len = 0;
}

void Compression(Files *f, int shift) {
    Tree *huffmanTree;
    size_t countFileLen = 0;
    huffmanTree = CreateHuffmanTree(f, &countFileLen);
    if (!huffmanTree)
        return;
    char **codeTable;
    Buff buff = {0};
    fwrite(&countFileLen, sizeof(int), 1, f->out);
    codeTable = CreateCodeTable(f, huffmanTree, &buff);
//    PrintTable(codeTable);
    fseek(f->in, shift, SEEK_SET);
    WriteC(f, codeTable, &buff);
    DeleteCodeTable(codeTable);
    DeleteTree(huffmanTree);
}

unsigned char ReadBit(Files *f, Buff *b) {
    if (!b->len) {
        b->buffer = (unsigned char) fgetc(f->in);
        b->len += 8;
    }
    b->len--;
    unsigned char resultBit = b->buffer >> 7;
    b->buffer <<= 1;
    return resultBit;
}

unsigned char ReadByte(Files *f, Buff *b) {
    if (!b->len) {
        b->buffer = (unsigned char) fgetc(f->in);
        b->len += 8;
    }
    unsigned char c = b->buffer;
    b->buffer = (unsigned char) fgetc(f->in);
    unsigned char cleanMask = ~((0177) >> b->len);
    unsigned char setMask = (b->buffer >> b->len);

    c &= cleanMask;
    c |= setMask;

    b->buffer = b->buffer << (8 - b->len);
    return c;
}

void RecreateTree(Files *f, Tree *tree, Buff *b) {
    if (ReadBit(f, b) == 0) {
        tree->left = CreateTree();
        RecreateTree(f, tree->left, b);
        tree->right = CreateTree();
        RecreateTree(f, tree->right, b);
    } else {
        tree->value = ReadByte(f, b);
        return;
    }
}

void WriteD(Files *f, Tree *tree, Buff *b) {
    if (!tree)
        return;
    if ((tree->right == NULL) && (tree->left == NULL)) {
        fwrite(&tree->value, sizeof(char), 1, f->out);
    } else {
        if (ReadBit(f, b)) {
            WriteD(f, tree->right, b);
        }
        else {
            WriteD(f, tree->left, b);
        }
    }
}

void Decompression(Files *f) {
    int fileLen = 0;
    if (!fread(&fileLen, sizeof(int), 1, f->in))
        return;
    Tree *huffmanTree = CreateTree();
    Buff *b = calloc(1, sizeof(Buff));
    RecreateTree(f, huffmanTree, b);
    while (fileLen) {
        WriteD(f, huffmanTree, b);
        fileLen--;
    }
    DeleteTree(huffmanTree);
    free(b);
}

int main(int argC, char *argV[]) {
    unsigned char mode;
    int shift = 0;
    Files *f;
    if (argC < 4) {
        f = OpenFiles();
        mode = (unsigned char) fgetc(f->in);
        shift = 1;
        fseek(f->in, shift, SEEK_SET);
    } else {
        f = calloc(1, sizeof(Files));
        f->in = fopen(argV[1], "rb");
        f->out = fopen(argV[2], "wb");
        mode = (unsigned char) argV[3][0];
    }
    switch (mode) {
        case COMPRESSION:
            Compression(f, shift);
            break;
        case DECOMPRESSION:
            Decompression(f);
            break;
        default:
            break;
    }
    CloseFiles(f);
    return 0;
}
