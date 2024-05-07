// Лабораторная работа № 0 - "Перевод вещественных чисел из одной системы счисления в другую"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFF_SIZE 128

const char *setChar = "0123456789ABCDEF";                  // допустимые символы с.с.

// проверяем символ ch на принадлежность диапазону 0..F и сис-ме счисления numberSys
int SymbolCorrect(char ch, int numberSys) {
    const char *res = strchr(setChar, toupper(ch));
    if (res == NULL) {
        return 0;
    } else {
        return (res - setChar < numberSys);
    }
}

// возвращает для ch соответствующее значение символа в с.с.
int Digit(char ch) {
    return strchr(setChar, toupper(ch)) - setChar;
}

int CheckBadInput(int b1, int b2, int lenNum, const char *num, int *countDot, int *posDot) {
    int flagError = 0;
    if (b1 < 2 || b1 > 16 || b2 < 2 || b2 > 16 || num[0] == '.' || num[lenNum - 1] == '.') {
        flagError = 1;
    } else {
        for (int i = 0; i < lenNum; ++i) {
            if (num[i] == '.') {
                ++(*countDot);
                *posDot = i + 1;
                if (*countDot > 1) {
                    flagError = 1;
                    break;
                }
            } else if (!SymbolCorrect(num[i], b1)) {
                flagError = 1;
                break;
            }
        }
    }
    return flagError;
}

// основная функция - переводит число num1 из с.с. b1 в с.с. b2
char *GetNumber(int b1, int b2, const char *num1, int lenNum1, int posDot, char *num2) {
    int lenInt = (posDot == 0) ? lenNum1 : posDot - 1;      // длина целой части num1
    int lenFrac = (posDot == 0) ? 0 : lenNum1 - posDot;     // длина дробной части num1
    long long int decInt = 0;                               // целая часть num1 в десятичной системе
    long double decFrac = 0;                                // дробная часть num1 в десятичной системе

    // переводим целую часть num1 в десятичную с.с.
    for (int i = 0; i < lenInt; ++i) {
        decInt = decInt * b1 + Digit(num1[i]);
    }

    // переводим дробную часть num1 в десятичную с.с.
    for (int i = lenNum1 - 1; i >= posDot; --i) {
        decFrac = (Digit(num1[i]) + decFrac) / b1;
    }

    // переводим десятичную целую часть num1 в систему счисления b2, записывая ее в num2 в обратном порядке
    int ind = 0;

    do {
        num2[ind++] = setChar[decInt % b2];
        decInt = decInt / b2;
    } while (decInt);

    // разворачиваем целую часть num2 в правильное положение
    for (int i = 0; i < ind / 2; ++i) {
        char ch = num2[i];
        num2[i] = num2[ind - i - 1];
        num2[ind - i - 1] = ch;
    }

    // переводим десятичную дробную часть num1 в систему счисления b2 и дописываем ее в num2
    if (lenFrac != 0) {
        num2[ind++] = '.';
        for (int i = 1; i <= 12; ++i) {
            long double comp = (decFrac * b2);
            num2[ind++] = setChar[(int) comp];
            decFrac = comp - (int) comp;
        }
    }

    // ставим символ завершения строки num2
    num2[ind] = '\0';

    return num2;
}

int main() {

    int b1;                         // исходная сис-ма счисления
    int b2;                         // конечная с.с.
    char num[BUFF_SIZE];                  // b1-ричная запись числа
    int lenNum = 0;                 // длина введённого числа b1 (длина строки)
    int countDot = 0;               // кол-во '.' в числе b1
    int posDot = 0;                 // номер символа '.' в числе b1 (не индекс)
    int flagError = 0;              // флаг ошибки

    if (scanf("%d%d%127s", &b1, &b2, num) < 3) {
        printf("scanf function error");
        return 0;
    }

    lenNum = strlen(num);
    flagError = CheckBadInput(b1, b2, lenNum, num, &countDot, &posDot);

    if (lenNum - countDot > 12 || flagError == 1) {
        printf("bad input");
    } else {
        char num2[BUFF_SIZE];
        printf("%s", GetNumber(b1, b2, num, lenNum, posDot, num2));
    }

    return 0;
}
