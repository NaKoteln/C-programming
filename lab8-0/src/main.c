#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

enum {
    OK = 0, BAD_VERTEX, BAD_LENGTH, BAD_NUMBER_OF_LINES, NOT_USED = -1
};

typedef struct {
    short int beginning;
    short int ending;
    int length;
} Edge;

void MakeSet(const int vertex, int *headOfSet) {
    headOfSet[vertex] = vertex;
}

int FindHead(const int vertex, int *headOfSet) {
    return (headOfSet[vertex] == vertex ? vertex : (headOfSet[vertex] = FindHead(headOfSet[vertex], headOfSet)));
}

void UnionSet(const int v1, const int v2, int *headOfSet) {
    int x = FindHead(v1, headOfSet);
    int y = FindHead(v2, headOfSet);
    if (x == y) {
        return;
    }
    headOfSet[y] = x;
}

int Compare(const void *x, const void *y) {
    Edge *a = (Edge *) x;
    Edge *b = (Edge *) y;
    return (a->length - b->length);
}

int FillArrayOfEdges(const int M, const int N, Edge *edges) {
    short int b, e;
    int l;
    for (int i = 0; i < M; ++i) {
        if (fscanf(stdin, "%hd %hd %d", &b, &e, &l) != 3) {
            return BAD_NUMBER_OF_LINES;
        };
        if (b < 1 || e < 1 || b > N || e > N) {
            return BAD_VERTEX;
        }
        if (l < 0) {
            return BAD_LENGTH;
        }
        edges[i].beginning = b;
        edges[i].ending = e;
        edges[i].length = l;
    }
    qsort(edges, M, sizeof(Edge), Compare);
    return OK;
}

bool DoKruskal(Edge *edges, const int M, const int N) {
    int headOfSet[N + 1];
    headOfSet[0] = N + 1;
    for (int i = 1; i <= N; ++i) {
        MakeSet(i, headOfSet);
    }
    for (int i = 0; i < M; ++i) {
        int b = FindHead(edges[i].beginning, headOfSet);
        int e = FindHead(edges[i].ending, headOfSet);
        if (b != e) {
            UnionSet(b, e, headOfSet);
        } else {
            edges[i].length = NOT_USED; //помечаем рёбра, которые не нужны в каркасе
        }
    }
    int headCounter = 0;
    for (int i = 1; i <= N; ++i) {
        if (headOfSet[i] == i) {
            headCounter++;
        }
    }
    return (headCounter == 1);
}

int main() {
    int N = 0;
    int M = 0;
    if (fscanf(stdin, "%d", &N) != 1) {
        return BAD_NUMBER_OF_LINES;
    }
    if (N < 0 || N > 5000) {
        fprintf(stdout, "bad number of vertices");
        return OK;
    }
    if (fscanf(stdin, "%d", &M) != 1) {
        return BAD_NUMBER_OF_LINES;
    }
    if (M < 0 || M > (N*(N+1)/2)) {
        fprintf(stdout, "bad number of edges");
        return OK;
    }
    Edge *edges = calloc(M, sizeof(Edge));
    assert(edges != NULL);
    switch (FillArrayOfEdges(M, N, edges)) {
        case BAD_VERTEX:
            fprintf(stdout, "bad vertex");
            free(edges);
            return OK;
        case BAD_LENGTH:
            fprintf(stdout, "bad length");
            free(edges);
            return OK;
        case BAD_NUMBER_OF_LINES:
            fprintf(stdout, "bad number of lines");
            free(edges);
            return OK;
        default:
            break;
    }
    if (DoKruskal(edges, M, N)) {
        for (int i = 0; i < M; ++i) {
            if (edges[i].length != -1) { //"-1" - данные рёбра не нужны в каркасе
                if (edges[i].beginning < edges[i].ending) {
                    fprintf(stdout, "%d %d\n", edges[i].beginning, edges[i].ending);
                } else {
                    fprintf(stdout, "%d %d\n", edges[i].ending, edges[i].beginning);
                }
            }
        }
    } else {
        fprintf(stdout, "no spanning tree");
    }
    free(edges);
    return OK;
}
