#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define MAX_VERTEXES 5000
#define INF (UINT_MAX)

static const int nothing = -1;
static const int emptyQueue = -1;
static const unsigned int notInQueue = INF - 1;

enum {
    OK = 0, BAD_VERTEX, BAD_LENGTH, BAD_NUMBER_OF_LINES
};

typedef int MyData;
typedef unsigned int PriorityType;

typedef struct _QNode {
    struct _QNode *next;
    MyData value;
    PriorityType priority;
} QNode;

typedef struct {
    QNode *beginning;

    PriorityType *priorities;
    int count;
} MinPriQueue;

typedef struct {
    int beginning;
    int ending;
    unsigned int length;
} Edge;

typedef struct _LNode {
    struct _LNode *next;
    Edge *edge;
} LNode;

typedef struct {
    LNode *beginning;
    int count;
    int vertex;
} EdgeList;

MinPriQueue *CreateQueue(int maxCapacity) {
    MinPriQueue *queue = calloc(1, sizeof(MinPriQueue));
    assert(queue != NULL);
    queue->priorities = malloc(maxCapacity * sizeof(PriorityType));
    assert(queue->priorities != NULL);
    memset(queue->priorities, INF, maxCapacity);
    return queue;
}

void DeleteNode(QNode *node) {
    free(node);
}

void DeleteQueue(MinPriQueue *queue) {
    QNode *tmp = queue->beginning;
    while (tmp) {
        QNode *next = tmp->next;
        DeleteNode(tmp);
        tmp = next;
    }
    free(queue->priorities);
    free(queue);
}

void AddToQueueNode(MinPriQueue *queue, QNode *node) {
    QNode *tmp = queue->beginning;
    QNode *parent = NULL;
    while (tmp) {
        if (node->priority > tmp->priority) {
            parent = tmp;
            tmp = tmp->next;
        } else {
            break;
        }
    }
    if (parent != NULL) {
        parent->next = node;
    } else {
        queue->beginning = node;
    }
    node->next = tmp;

    queue->priorities[node->value - 1] = node->priority;
    queue->count++;
}

MyData GetFromQueue(MinPriQueue *queue) {
    if (queue->count == 0) {
        return emptyQueue;
    }
    MyData value = queue->beginning->value;
    queue->priorities[value - 1] = notInQueue;
    QNode *node = queue->beginning->next;
    DeleteNode(queue->beginning);
    queue->beginning = node;
    queue->count--;
    return value;
}

QNode *CreateNode(MyData value, PriorityType priority) {
    QNode *node = calloc(1, sizeof(QNode));
    assert(node != NULL);
    node->value = value;
    node->priority = priority;
    return node;
}

QNode *GetNode(MinPriQueue *queue, MyData value) {
    QNode *tmp = queue->beginning;
    QNode *parent = NULL;
    while (tmp && tmp->value != value) {
        parent = tmp;
        tmp = tmp->next;
    }
    if (tmp) {
        if (parent) {
            parent->next = tmp->next;
        } else {
            queue->beginning = tmp->next;
        }
        queue->priorities[tmp->value - 1] = notInQueue;
        queue->count--;
    }
    return tmp;
}

void UpdatePriority(MinPriQueue *queue, MyData value, PriorityType priority) {
    QNode *node = GetNode(queue, value);
    if (node) {
        node->priority = priority;
        AddToQueueNode(queue, node);
    }
}

void AddToQueueElement(MinPriQueue *queue, MyData value, PriorityType priority) {
    QNode *node = CreateNode(value, priority);
    AddToQueueNode(queue, node);
}

PriorityType GetPriority(MinPriQueue *queue, MyData value) {
    return queue->priorities[value - 1];
}

EdgeList *CreateEdgeList(int vertex) {
    EdgeList *edgeList = calloc(1, sizeof(EdgeList));
    assert(edgeList != NULL);
    edgeList->vertex = vertex;
    return edgeList;
}

Edge *GetEdge(EdgeList *edgeList) {
    LNode *node = edgeList->beginning;
    Edge *edge = NULL;
    if (node) {
        edgeList->beginning = node->next;
        edgeList->count--;
        edge = node->edge;
        free(node);
    }
    return edge;
}

void DeleteEdge(Edge *edge) {
    free(edge);
}

void DeleteEdgeList(EdgeList *edgeList) {
    while (edgeList->count) {
        DeleteEdge(GetEdge(edgeList));
    }
    free(edgeList);
}

void AddToListEdge(EdgeList *edgeList, Edge *edge) {
    LNode *node = calloc(1, sizeof(LNode));
    assert(node != NULL);
    node->edge = edge;
    node->next = edgeList->beginning;
    edgeList->beginning = node;
    edgeList->count++;
}

void PrintList(EdgeList *edgeList) {
    LNode *node = edgeList->beginning;
    for (int i = 0; i < edgeList->count; ++i) {
        fprintf(stdout, "%d %d\n", node->edge->beginning, node->edge->ending);
        node = node->next;
    }
}

Edge *CreateEdge(int b, int e, unsigned int l) {
    Edge *edge = malloc(sizeof(Edge));
    assert(edge != NULL);
    edge->beginning = b;
    edge->ending = e;
    edge->length = l;
    return edge;
}

void DeleteEdgeLists(EdgeList **edgeLists, int n) {
    for (int i = 0; i < n; ++i) {
        DeleteEdgeList(edgeLists[i]);
    }
    free(edgeLists);
}


int FillEdgeList(int M, int N, EdgeList **edgeLists) {
    int b, e;
    unsigned int l;
    for (int i = 0; i < M; ++i) {
        if (fscanf(stdin, "%d %d %u", &b, &e, &l) != 3) {
            return BAD_NUMBER_OF_LINES;
        }
        if (b < 1 || e < 1 || b > N || e > N) {
            return BAD_VERTEX;
        }
        if (l > INT_MAX) {
            return BAD_LENGTH;
        }
        AddToListEdge(edgeLists[b - 1], CreateEdge(b, e, l));
        AddToListEdge(edgeLists[e - 1], CreateEdge(e, b, l));
    }
    return OK;
}

int FillEdgeTable(int M, int N, unsigned int **edgeTable) {
    int b, e;
    unsigned int l;
    for (int i = 0; i < M; ++i) {
        if (fscanf(stdin, "%d %d %u", &b, &e, &l) != 3) {
            return BAD_NUMBER_OF_LINES;
        }
        if (b < 1 || e < 1 || b > N || e > N) {
            return BAD_VERTEX;
        }
        if (l > INT_MAX) {
            return BAD_LENGTH;
        }
        edgeTable[b - 1][e - 1] = l;
        edgeTable[e - 1][b - 1] = l;
    }
    return OK;
}

EdgeList *DoPrimLists(EdgeList **edgeLists, int N) {
    EdgeList *resultList = CreateEdgeList(0);
    MinPriQueue *queue = CreateQueue(N);
    int parent[N];
    for (int i = 1; i <= N; ++i) {
        AddToQueueElement(queue, i, INF);
        parent[i - 1] = nothing;
    }
    UpdatePriority(queue, 1, 0);

    int v = GetFromQueue(queue);
    while (queue->count) {
        int k = edgeLists[v - 1]->count;
        for (int i = 0; i < k; ++i) {
            assert(v == edgeLists[v - 1]->vertex);
            Edge *edge = GetEdge(edgeLists[v - 1]);
            int u = edge->ending;
            PriorityType p = GetPriority(queue, u);
            if (p != notInQueue && edge->length < p) {
                UpdatePriority(queue, u, edge->length);
                parent[u - 1] = v;
            }
            DeleteEdge(edge);
        }
        v = GetFromQueue(queue);
        if (parent[v - 1] != nothing) {
            AddToListEdge(resultList, CreateEdge(parent[v - 1], v, 45));
        }
    }
    DeleteQueue(queue);
    return resultList;
}

EdgeList *DoPrimTable(int N, unsigned int **edgeTable) {
    EdgeList *resultList = CreateEdgeList(0);
    MinPriQueue *queue = CreateQueue(N);
    int parent[N];
    for (int i = 1; i <= N; ++i) {
        AddToQueueElement(queue, i, INF);
        parent[i - 1] = nothing;
    }
    UpdatePriority(queue, 1, 0);

    int v = GetFromQueue(queue);
    while (queue->count) {
        for (int i = 1; i <= N; ++i) {
            unsigned int len = edgeTable[v - 1][i - 1];
            if (len) {
                PriorityType p = GetPriority(queue, i);
                if (p != notInQueue && len < p) {
                    UpdatePriority(queue, i, len);
                    parent[i - 1] = v;
                }
            }
        }
        v = GetFromQueue(queue);
        if (parent[v - 1] != nothing) {
            AddToListEdge(resultList, CreateEdge(parent[v - 1], v, 45));
        }
    }
    DeleteQueue(queue);
    return resultList;
}

void PrimList(int N, int M) {
    EdgeList **edgeLists = calloc(N, sizeof(EdgeList *));
    assert(edgeLists != NULL);
    for (int i = 0; i < N; ++i) {
        edgeLists[i] = CreateEdgeList(i + 1);
    }

    switch (FillEdgeList(M, N, edgeLists)) {
        case BAD_VERTEX:
            fprintf(stdout, "bad vertex");
            DeleteEdgeLists(edgeLists, N);
            return;
        case BAD_LENGTH:
            fprintf(stdout, "bad length");
            DeleteEdgeLists(edgeLists, N);
            return;
        case BAD_NUMBER_OF_LINES:
            fprintf(stdout, "bad number of lines");
            DeleteEdgeLists(edgeLists, N);
            return;
        default:
            break;
    }

    EdgeList *list = DoPrimLists(edgeLists, N);
    if (list->count != (N - 1)) {
        fprintf(stdout, "no spanning tree");
    } else {
        PrintList(list);
    }
    DeleteEdgeList(list);
    DeleteEdgeLists(edgeLists, N);
}

void DeleteEdgeTable(unsigned int **edgeTable, int N) {
    for (int i = 0; i < N; ++i) {
        free(edgeTable[i]);
    }
    free(edgeTable);
}

void PrimTable(int N, int M) {
    unsigned int **edgeTable = malloc(N * sizeof(int *));
    assert(edgeTable != NULL);
    for (int i = 0; i < N; ++i) {
        edgeTable[i] = malloc(N * sizeof(int));
        assert(edgeTable[i] != NULL);
        for (int j = 0; j < N; ++j) {
            edgeTable[i][j] = INF;
        }
    }

    switch (FillEdgeTable(M, N, edgeTable)) {
        case BAD_VERTEX:
            fprintf(stdout, "bad vertex");
            DeleteEdgeTable(edgeTable, N);
            return;
        case BAD_LENGTH:
            fprintf(stdout, "bad length");
            DeleteEdgeTable(edgeTable, N);
            return;
        case BAD_NUMBER_OF_LINES:
            fprintf(stdout, "bad number of lines");
            DeleteEdgeTable(edgeTable, N);
            return;
        default:
            break;
    }

    EdgeList *list = DoPrimTable(N, edgeTable);
    if (list->count != (N - 1)) {
        fprintf(stdout, "no spanning tree");
    } else {
        PrintList(list);
    }
    DeleteEdgeList(list);

    DeleteEdgeTable(edgeTable, N);
}

int main() {
    int N = 0;
    int M = 0;
    if (fscanf(stdin, "%d", &N) != 1) {
        return BAD_NUMBER_OF_LINES;
    }
    if (N < 0 || N > MAX_VERTEXES) {
        fprintf(stdout, "bad number of vertices");
        return OK;
    }
    if (fscanf(stdin, "%d", &M) != 1) {
        return BAD_NUMBER_OF_LINES;
    }
    if (M < 0 || M > (N * (N + 1) / 2)) {
        fprintf(stdout, "bad number of edges");
        return OK;
    }
    if (M < N * N / 8) {
        PrimList(N, M);
    } else {
        PrimTable(N, M);
    }
    return OK;
}
