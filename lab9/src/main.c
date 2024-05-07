#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

#define INFINITY UINT_MAX

enum {
    OK = 0, BAD_VERTEX, BAD_LENGTH, BAD_NUMBER_OF_LINES
};

typedef struct {
    char visited;           //посещённая или непосещённая вершина
    unsigned int mark;      //метка - минимальное известное расстояние
    int parent;             //вершина, от которой построен минимальный путь
} Vertex;

void InitialGraph(const int N, Vertex *vertex, unsigned int *adjTable) {
    for (int i = 0; i <= N; ++i) {
        vertex[i].visited = 0;
        vertex[i].mark = INFINITY;
        vertex[i].parent = 0;
        for (int j = 0; j <= N; ++j) {
            adjTable[i * (N + 1) + j] = INFINITY;
        }
    }
}

int FillingTheAdjTable(const int N, const int M, unsigned int *adjTable) {
    int b, e;
    long long int l;
    for (int i = 0; i < M; ++i) {
        if (fscanf(stdin, "%d %d %lld", &b, &e, &l) != 3) {
            return BAD_NUMBER_OF_LINES;
        }
        if (b < 1 || e < 1 || b > N || e > N) {
            return BAD_VERTEX;
        }
        if (l < 0 || l > INT_MAX) {
            return BAD_LENGTH;
        }
        adjTable[b * (N + 1) + e] = (unsigned int) l;
        adjTable[e * (N + 1) + b] = adjTable[b * (N + 1) + e];
    }
    return OK;
}

//от вершины S до каждой вершины графа, вершины с 1
void CalculationShortestPaths(const int S, const int N, int *count, Vertex *vertex, unsigned int *adjTable) {
    vertex[S].mark = 0;
    for (int i = 1; i <= N; ++i) {
        //находим вершину с минимальным расстоянием
        int u = 0;           
        for (int j = 0; j <= N; ++j) {
            if ((vertex[j].mark < vertex[u].mark) && vertex[j].visited == 0) {
                u = j;
            }
        }
        if (u == 0) {
            break;
        }
        vertex[u].visited = 1;
        for (int j = 1; j <= N; ++j) {
            if (vertex[j].visited != 1 && adjTable[u * (N + 1) + j] != INFINITY) {
                unsigned int p = vertex[u].mark + adjTable[u * (N + 1) + j];
                if (vertex[j].mark > p) {
                    vertex[j].parent = u;
                    vertex[j].mark = p;
                }
                if (p > INT_MAX) {
                    *count += 1;
                }
            }
        }
    }
}

void PrintResult(const int N, const int F, const int count, Vertex *vertex) {
    for (int i = 1; i <= N; ++i) {
        if (vertex[i].mark == INFINITY) {
            fprintf(stdout, "oo ");
        } else if (vertex[i].mark > INT_MAX) {
            fprintf(stdout, "INT_MAX+ ");
        } else {
            fprintf(stdout, "%u ", vertex[i].mark);
        }
    }
    printf("\n");
    if (vertex[F].mark == INFINITY) {
        fprintf(stdout, "no path");
    } else if (vertex[F].mark > INT_MAX && count > 1) {
        fprintf(stdout, "overflow");
    } else {
        int x = F;
        while (x != 0) {
            fprintf(stdout, "%d ", x);
            x = vertex[x].parent;
        }
    }
}

int main() {
    int N = 0, M = 0;
    int S = 0, F = 0;
    if (fscanf(stdin, "%d", &N) != 1) {
        fprintf(stdout, "bad number of lines");
        return 0;
    }
    if (N < 0 || N > 5000) {
        fprintf(stdout, "bad number of vertices");
        return OK;
    }
    if (fscanf(stdin, "%d %d", &S, &F) != 2) {
        fprintf(stdout, "bad number of lines");
        return 0;
    }
    if (S < 1 || S > N || F < 1 || F > N) {
        fprintf(stdout, "bad vertex");
        return OK;
    }
    if (fscanf(stdin, "%d", &M) != 1) {
        fprintf(stdout, "bad number of lines");
        return 0;
    }
    if (M < 0 || M > (N * (N + 1) / 2)) {
        fprintf(stdout, "bad number of edges");
        return OK;
    }
    Vertex *vertex = (Vertex *) calloc(N + 1, sizeof(Vertex)); //под +1 вершину для удобства работы с индексами
    assert(vertex != NULL);
    unsigned int *adjTable = calloc((N + 1) * (N + 1), sizeof(unsigned int));
    assert(adjTable != NULL);
    InitialGraph(N, vertex, adjTable);
    switch (FillingTheAdjTable(N, M, adjTable)) {
        case BAD_VERTEX:
            fprintf(stdout, "bad vertex");
            free(adjTable);
            free(vertex);
            return OK;
        case BAD_LENGTH:
            fprintf(stdout, "bad length");
            free(adjTable);
            free(vertex);
            return OK;
        case BAD_NUMBER_OF_LINES:
            fprintf(stdout, "bad number of lines");
            free(adjTable);
            free(vertex);
            return OK;
        default:
            break;
    }
    int countOverflow = 0;
    CalculationShortestPaths(S, N, &countOverflow, vertex, adjTable);
    PrintResult(N, F, countOverflow, vertex);
    free(adjTable);
    free(vertex);
    return 0;
}
