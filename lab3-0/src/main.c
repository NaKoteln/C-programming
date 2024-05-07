#include <stdio.h>
#include <stdlib.h>

int FillArray(int *arr, int count) {
    int sum = 0;
    for (int i = 0; i < count; ++i) {
        if (scanf("%d", &arr[i]) == 1) {
            sum++;
        }
    }
    
    return sum;
}

void SwapItem(int *num1, int *num2) {
    int c = *num1;
    *num1 = *num2;
    *num2 = c;
}

void CreateHeap(int *arr, int root, int count) {
    int maxChild;
    while (root * 2 <= count) {

        if ((root * 2 == count) || (arr[root * 2] > arr[root * 2 + 1])) {
            maxChild = root * 2;
        } else {
            maxChild = root * 2 + 1;
        }

        if (arr[root] < arr[maxChild]) {
            SwapItem(&arr[root], &arr[maxChild]);
            root = maxChild;
        } else {
            break;
        }

    }
}

void HeapSort(int *arr, int count) {
    for (int i = (count / 2); i >= 0; --i) {
        CreateHeap(arr, i, count-1);
    }

    for (int i = count - 1; i > 0; --i) {
        SwapItem(&arr[i], &arr[0]);
        CreateHeap(arr, 0, i - 1);
    }
}

void PrintArray(int *arr, int count) {
    for (int i = 0; i < count; ++i) {
        printf("%d ", arr[i]);
    }
}

int main() {

    int size = 0;
    if ((scanf("%d", &size) < 1) || (size <= 0)) {
        return 0;
    }

    int *numArray = (int *) calloc(size, sizeof(int));

    if (numArray == NULL) {
        return 0;
    } else if (FillArray(numArray, size) < size) {
        free(numArray);
        return 0;
    }
    

    HeapSort(numArray, size);

    PrintArray(numArray, size);
    
    free(numArray);

    return 0;
}
