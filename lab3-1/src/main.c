// Лабораторная работа №3-1 - "Быстрая сортировка с разбиением Хоара"
#include <stdio.h>
#include <stdlib.h>

void SwapItem(int *num1, int *num2) {
    int c = *num1;
    *num1 = *num2;
    *num2 = c;
}

void SortHoare(int *arr, int left, int right) {

    int i = left;                                   // индекс первого элемента массива arr
    int j = right;                                  // индекс последнего элемента массива arr
    int pivot = (arr[left] + arr[right]) / 2;       // значение опорного элемента

    while (i <= j) {

        while (arr[i] < pivot) {                    // ищем слева элемент больше опорного
            ++i;
        }

        while (arr[j] > pivot) {                    // ищем справа элемент меньше опорного
            --j;
        }

        if (i <= j) {                               // меняем элементы местами
            SwapItem(&arr[i++], &arr[j--]);
        }
    }

    if (left < j) {                                 // сортируем левую часть массива
        SortHoare(arr, left, j);
    }
    if (i < right) {                                // сортируем правую часть массива
        SortHoare(arr, i, right);
    }

}

int main() {

    int size;
    if (scanf("%d", &size) < 1) {
        return 0;
    } else if (size <= 0) {
        return 0;
    }

    int *numArray = (int *) calloc(size, sizeof(int));
    if (numArray == NULL) {
        return 0;
    } else {
        for (int i = 0; i < size; ++i) {
            if (scanf("%d", &numArray[i]) < 1) {
                free(numArray);
                return 0;
            }
        }
    }

    SortHoare(numArray, 0, size - 1);

    for (int i = 0; i < size; ++i) {
        printf("%d ", numArray[i]);
    }

    free(numArray);

    return 0;
}
