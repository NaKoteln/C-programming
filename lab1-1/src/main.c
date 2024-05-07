#include <stdio.h>
#include <string.h>

#define MAX_TEMPLATE 16

unsigned int FillingTemplate(unsigned char *template) {
    unsigned int lengthTemplate = 0;
    for (int i = 0; i < MAX_TEMPLATE + 1; i++) {
        template[i] = (char) getchar();
        if (template[i] == '\n') {
            break;
        }
        lengthTemplate += 1;
    }
    return lengthTemplate;
}

int PowerOf3(unsigned int lenTemplate) {
    int hashDegree = 1;
    for (unsigned int i = 0; i < lenTemplate - 1; ++i) {
        hashDegree *= 3;
    }
    return hashDegree;
}

unsigned int CalculateHash(const unsigned char *array, unsigned int size) {
    int hash = 0;
    int degree = 1;
    for (unsigned int i = 0; i < size; ++i) {
        hash += ((int) array[i] % 3) * degree;
        degree *= 3;
    }
    return hash;
}

unsigned int RecalculateHash(unsigned int textHash, int degree, unsigned char ch1, unsigned char ch2) {
    textHash -= (int) ch1 % 3;
    textHash /= 3;
    textHash += (int) ch2 % 3 * degree;
    return textHash;
}

void RabinKarpAlgorithm(const unsigned char *template, unsigned int lenTemplate) {
    unsigned int templateHash = CalculateHash(template, lenTemplate);
    printf("%u ", templateHash);

    int hashDegree = PowerOf3(lenTemplate);

    unsigned char buff[256];
    memset(buff, 0, lenTemplate);
    unsigned int lenText = 0;

    while (fgets((char *) buff, 256, stdin) != NULL) {
        unsigned int lenString = strlen((char *) buff);

        if (lenString + 1 > lenTemplate) {
            unsigned int k = lenTemplate;
            unsigned int textHash = CalculateHash(buff, lenTemplate);
            while (k < lenString + 1) {
                if (textHash == templateHash) {
                    for (unsigned int i = k - lenTemplate; i < k; i++) {
                        printf("%u ", lenText + i + 1);
                        if (buff[i] != template[i + lenTemplate - k]) {
                            break;
                        }
                    }
                }
                textHash = RecalculateHash(textHash, hashDegree, buff[k - lenTemplate], buff[k]);
                k += 1;
            }
        }
        lenText += lenString;
    }
}

int main() {
    unsigned char template[MAX_TEMPLATE + 1] = {0};
    unsigned int lenTemplate = FillingTemplate(template);

    RabinKarpAlgorithm(template, lenTemplate);
    return 0;
}
