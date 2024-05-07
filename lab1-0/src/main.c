#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TEMPLATE 17

unsigned int FillingTemplate(unsigned char *template) {
    unsigned int lengthTemplate = 0;
    for (int i = 0; i < MAX_TEMPLATE; ++i) {
        template[i] = (char) getchar();
        if (template[i] == '\n') {
            break;
        }
        lengthTemplate += 1;
    }
    return lengthTemplate;
}

void BuildingShiftTable(unsigned int *tableShift, unsigned int lenTemplate, const unsigned char *template) {
    for (unsigned int i = 0; i < lenTemplate - 1; i++) {
        unsigned int k = template[i];
        tableShift[k] = lenTemplate - i - 1;
    }
}

size_t ShiftBuffer(unsigned char *buff, unsigned int n, unsigned int lenBuff) {
    memmove(buff, &buff[n], lenBuff - n);
    size_t read = fread(&buff[lenBuff - n], sizeof(char), n, stdin);
    return read;
}

void TemplateAndTextComparison(unsigned int lenTemplate, const unsigned int *tableShift, const unsigned char *template) {
    unsigned int lenText = 0; //длина считанного текста
    unsigned char buff[lenTemplate]; // строки текста
    memset(buff, 0, lenTemplate);
    unsigned int needToRead = lenTemplate;
    unsigned int startPosition = lenTemplate;
    while (ShiftBuffer(buff, needToRead, lenTemplate) == needToRead) {
        unsigned int i;
        for (i = startPosition; i > 0; i--) {
            printf("%u ", lenText + i);
            if (buff[i - 1] != template[i - 1]) {
                break;
            }
        }

        int k = buff[lenTemplate - 1];
        if (i == 0 || tableShift[k] == 0) {
            needToRead = lenTemplate;
        } else {
            needToRead = tableShift[k];
        }
        lenText += needToRead;
    }
}

int main() {

    unsigned char template[MAX_TEMPLATE] = {0};
    unsigned int tableOfShift[256] = {0}; // таблица сдвигов, индекс - номер элемента по ASCII
    unsigned int lengthTemplate = FillingTemplate(template); // длина шаблона
    if (lengthTemplate < 1 || lengthTemplate > MAX_TEMPLATE) {
        return 0;
    }
    // строим таблицу сдвигов (до предпоследнего элемента)
    BuildingShiftTable(tableOfShift, lengthTemplate, template);
    // главная часть - сравнение текущего текста и шаблона
    TemplateAndTextComparison(lengthTemplate, tableOfShift, template);

    return EXIT_SUCCESS;
}
