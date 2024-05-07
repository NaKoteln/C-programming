#include <stdio.h>
#include <stdlib.h>

#define WHITE (0)
#define GRAY (1)
#define BLACK (2)
#define BAD_VERTEX (-1)
#define BAD_NUMBER_OF_LINES (-2)
#define OK (0)

unsigned char GetBitIdx(const unsigned char* array, int x, int y, int N) {
    int idx = x * N + y;
    unsigned char bitMask = (1 << (7 - idx % 8));
    return array[idx / 8] & bitMask;
}

void SetBitIdx(unsigned char* array, int x, int y, int N, char value) {
    int idx = x * N + y;
    unsigned char v = value != 0;
    unsigned char bitMask = (v << (7 - idx % 8));
    v ? (array[idx / 8] |= bitMask) : (array[idx / 8] &= ~bitMask);
}

typedef struct {
    unsigned char *vertexes;  // матрица смежности
    int N;
} Graph;

void AddEdge(Graph *graph, int b, int e) {
    SetBitIdx(graph->vertexes, b, e, graph->N, 1);
}

void InitialGraph(Graph *graph, int N) {
    graph->vertexes = (unsigned char *) calloc(N * N / 8 + 1, sizeof(unsigned char));
    graph->N = N;
}

int FillingInTheGraph(int N, int M, Graph *graph) {
    int b, e;
    for (int i = 0; i < M; ++i) {
        if (fscanf(stdin, "%d %d", &b, &e) != 2) {
            return BAD_NUMBER_OF_LINES;
        }
        if (b < 1 || e < 1 || b > N || e > N) {
            return BAD_VERTEX;
        }
        AddEdge(graph, b-1, e-1);
    }
    return OK;
}

void FreeGraph(Graph *graph) {
    free(graph->vertexes);
    free(graph);
}

void DepthFirstTraversal(Graph *graph, unsigned char *colour, int b, unsigned int *stack, int *sortIsPossible) {
    static int k = 0;
    if (colour[b] == GRAY) {
        *sortIsPossible = 0;
        return;
    } else if (colour[b] == WHITE) {
        colour[b] = GRAY;
        for (int i = 0; i < graph->N; ++i) {
            if(GetBitIdx(graph->vertexes, b, i, graph->N)) {
                DepthFirstTraversal(graph, colour, i, stack, sortIsPossible);
            }
        }
        colour[b] = BLACK;
        stack[k++] = b;
    }
}

void TopologicalSort(Graph *graph, int N, unsigned int *stack, int *sortIsPossible) {
    unsigned char *colour = (unsigned char *) calloc(N, sizeof(unsigned char)); // массив цветов
    for (int i = 0; i < N; i++) {
        colour[i] = WHITE;
    }
    for (int i = 0; i < N; i++) {
        if (*sortIsPossible) {
            DepthFirstTraversal(graph, colour, i, stack, sortIsPossible);
        } else {
            break;
        }
    }
    free(colour);
}

int main() {
    int N = 0;
    int M = 0;
    if (fscanf(stdin, "%d\n", &N) != 1) {
        fprintf(stdout, "bad number of lines");
        return 0;
    }
    if (N < 0 || N > 2000) {
        fprintf(stdout, "bad number of vertices");
        return OK;
    }
    if (fscanf(stdin, "%d\n", &M) != 1) {
        fprintf(stdout, "bad number of lines");
        return 0;
    }
    if (M < 0 || M > (N * (N + 1) / 2)) {
        fprintf(stdout, "bad number of edges");
        return OK;
    }

    Graph *graph = calloc(N, sizeof(Graph));
    int sortingIsPossible = 1;
    InitialGraph(graph, N);
    switch (FillingInTheGraph(N, M, (Graph *) graph)) {
        case BAD_VERTEX:
            fprintf(stdout, "bad vertexes");
            FreeGraph(graph);
            return 0;
        case BAD_NUMBER_OF_LINES:
            fprintf(stdout, "bad number of lines");
            FreeGraph(graph);
            return 0;
        default:
            break;
    }
    unsigned int *stack = (unsigned int *) calloc(N, sizeof(unsigned int));
    TopologicalSort(graph, N, stack, &sortingIsPossible);
    if (sortingIsPossible) {
        for (int i = N - 1; i >= 0; i--) {
            printf("%u ", stack[i] + 1);
        }
    } else {
        printf("impossible to sort");
    }
    free(stack);
    FreeGraph(graph);
    return 0;
}
