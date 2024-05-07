#include <stdio.h>
#include <string.h>

const char *setChar = "0123456789";

void PrintArray(char *symbols, int countSymbols) {
    for (int i = 0; i < countSymbols; ++i) {
        printf("%d", symbols[i] - '0');
    }
    printf("\n");
}

int ScanPermutation(char *symbols) {

    int countSymbols = 0;
    char symbol;

    while ((symbol = getchar()) != '\n') {
        if (strchr(setChar, symbol) != NULL) {
            countSymbols++;
            if (countSymbols > 10) {
                return 0;
            } else {
                symbols[countSymbols - 1] = symbol;
            }
        } else {
            return 0;
        }
    }

    for (int i = 1; i < countSymbols; ++i) {
        for (int j = 0; j < i; ++j) {
            if (symbols[j] == symbols[i]) {
                return 0;
            }
        }
    }

    return countSymbols;
}

void SwapItem(char *num1, char *num2) {
    char c = *num1;
    *num1 = *num2;
    *num2 = c;
}

int CountDescents(char *symbols, int countSymbols) {
    int count = 0;
    for (int i = 1; i < countSymbols; ++i) {
        if (symbols[i - 1] > symbols[i]) {
            count++;
        }
    }
    return count;
}

void PermutationGenerator(char *symbols, int countPermutation, int countSymbols) {

    if (countPermutation <= 0) {
        return;
    } else {
        while (countPermutation > 0) {
            if (CountDescents(symbols, countSymbols) == countSymbols - 1) {
                return;
            }
           
            int largestIncreasing = 0;
            for (int i = 1; i < countSymbols; ++i) {
                if (symbols[i - 1] < symbols[i]) {
                    largestIncreasing = i - 1;
                }
            }
            
            int largestElement = 0;
            for (int i = largestIncreasing + 1; i < countSymbols; ++i) {
                if (symbols[i] > symbols[largestIncreasing]) {
                    largestElement = i;
                }
            }

            SwapItem(&symbols[largestIncreasing], &symbols[largestElement]);

            for (int i = largestIncreasing + 1; i <= countSymbols / 2; ++i) {
                SwapItem(&symbols[i], &symbols[countSymbols - i]);
            }

            PrintArray(symbols, countSymbols);
            countPermutation--;
        }
    }
}

int main() {

    char symbols[10];
    int count = ScanPermutation(symbols);
    int N;
    if (scanf("%d", &N) < 1) {
        return 0;
    }

    if (count == 0) {
        printf("bad input");
    } else {
        PermutationGenerator(symbols, N, count);
    }

    return 0;
}
