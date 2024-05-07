#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define SYNTAX_ERROR {printf("syntax error\n"); return -1;}
#define DIVISION_ZERO {printf("division by zero"); return EXIT_SUCCESS;}

#define OPERATION true
#define DIGIT false

enum LastElement {
    START_FILE,
    OPERATOR,
    NUMBER,
    OPEN_BRACKET,
    CLOSE_BRACKET
};

typedef struct StackElement {
    int value;
    bool isOperation; // если 1, то value - операция, иначе - число
} Stack;

typedef struct StackOperand {
    struct StackOperand *next;
    int value;
} ListOperand;

typedef struct StackOperator {
    struct StackOperator *next;
    char operator;
} ListOperator;

void PushOperand(ListOperand **list, int data) {
    ListOperand *newList = malloc(sizeof(ListOperand));
    newList->value = data;
    newList->next = *list;
    *list = newList;
}
void PushOperator(ListOperator **list, char data) {
    ListOperator *newList = malloc(sizeof(ListOperator));
    newList->operator = data;
    newList->next = *list;
    *list = newList;
}

int PopOperand(ListOperand **list) {
    ListOperand *removeElement = *list;
    int data = removeElement->value;
    *list = (*list)->next;
    free(removeElement);
    return data;
}

char PopOperator(ListOperator **list) {
    ListOperator  *removeElement = *list;
    char data = removeElement->operator;
    *list = (*list)->next;
    free(removeElement);
    return data;
}

void FreeListOperator(ListOperator **list)
{
    if(list != NULL)
    {
        FreeListOperator(&((*list)->next));
        free(*list);
    }
}

void FreeListOperand(ListOperand **list)
{
    if(list != NULL)
    {
        FreeListOperand(&((*list)->next));
        free(*list);
    }
}

bool IsOperation(char symbol) {
    return (symbol == '+' || symbol == '-' || symbol == '*' || symbol == '/');
}

bool IsDigit(char symbol) {
    return (symbol - '0' <= 9 && symbol - '0' >= 0);
}

int PriorityOperation(char operation) {
    switch (operation) {
        case '*':
        case '/':
            return 2;
        case '+':
        case '-':
            return 1;
        default:
            return 0;
    }
}

int CreatePostfixNote(char *expression, Stack *stack) {

    ListOperator *operatorsStack = NULL;
    int stackPosition = 0;
    int position = 0;

    // нужно знать предыдущий символ, т.к. возможны ошибки ошибки:
    // 2 4, ) 4, + +, (), + ), etc
    enum LastElement lastElement = START_FILE;
    while (expression[position] != '\n') {
        if (IsDigit(expression[position])) { //полученный символ - цифра
            if (lastElement == NUMBER || lastElement == CLOSE_BRACKET) { // 2 3 || ) 2
                FreeListOperator(&operatorsStack);
                SYNTAX_ERROR
            }
            // перевод цифр, идущих подряд, в число
            int number = 0;
            while (IsDigit(expression[position]) == 1) {
                number = number * 10 + (expression[position] - '0');
                position += 1;
            }
            position -= 1;
            Stack element = {number, DIGIT};
            stack[stackPosition] = element;
            stackPosition += 1;
            lastElement = NUMBER;
        } else if (expression[position] == '(') { // полученный символ - скобка, начинающая выражение
            if (lastElement == NUMBER) {  // 7 (
                FreeListOperator(&operatorsStack);
                SYNTAX_ERROR
            }
            PushOperator(&operatorsStack, expression[position]);
            lastElement = OPEN_BRACKET;
        } else if (expression[position] == ')') { // полученный символ - скобка, закрывающая выражение
            if (lastElement == OPEN_BRACKET || lastElement == OPERATOR || lastElement == START_FILE) { // () || * )
                FreeListOperator(&operatorsStack);
                SYNTAX_ERROR
            }
            // нужно проверить наличие открывающих скобок
            bool isExpression = false;
            while (operatorsStack != NULL) {
                if (operatorsStack->operator == '(') {
                    isExpression = true; // возможна ошибка
                    break;
                }
                Stack element = {operatorsStack->operator, OPERATION};
                stack[stackPosition] = element;
                PopOperator(&operatorsStack);
                stackPosition += 1;
            }
            if (isExpression == false) {
                FreeListOperator(&operatorsStack);
                SYNTAX_ERROR
            }
            PopOperator(&operatorsStack);
            lastElement = CLOSE_BRACKET;
        } else if (IsOperation(expression[position])) { // полученный символ - оператор
            if (lastElement == OPEN_BRACKET || lastElement == START_FILE) {
                // нужно учесть и обработать отрицательные числа
                if (expression[position] == '-') {
                    Stack element = {0, DIGIT};
                    stack[stackPosition] = element;
                    stackPosition += 1;
                } else {
                    FreeListOperator(&operatorsStack);
                    SYNTAX_ERROR
                }
            }
            if (lastElement == OPERATOR) { // + +
                FreeListOperator(&operatorsStack);
                SYNTAX_ERROR
            }
            //сравниваем приоритет операции с приоритетом предыдущей операции и заполняем стек операторов
            if (operatorsStack != NULL) {
                if (PriorityOperation(expression[position]) <=
                    PriorityOperation(operatorsStack->operator)) {
                    Stack element = {operatorsStack->operator, OPERATION};
                    stack[stackPosition] = element;
                    stackPosition += 1;
                    PopOperator(&operatorsStack);
                }
            }
            PushOperator(&operatorsStack, expression[position]);
            lastElement = OPERATOR;
        } else { // полученный символ - посторонний
            FreeListOperator(&operatorsStack);
            SYNTAX_ERROR
        }
        position += 1;
    }

    if (lastElement == OPERATOR || lastElement == OPEN_BRACKET) { // ".. * " || ".. ("
        FreeListOperator(&operatorsStack);
        SYNTAX_ERROR
    }
    while (operatorsStack != NULL) {
        if (operatorsStack->operator == OPEN_BRACKET) {
            FreeListOperator(&operatorsStack);
            SYNTAX_ERROR
        }
        Stack element = {operatorsStack->operator, OPERATION};
        PopOperator(&operatorsStack);
        stack[stackPosition++] = element;
    }
    Stack element = {0, OPERATION};
    stack[stackPosition] = element;
    FreeListOperator(&operatorsStack);
    return 1;
}

int SizeStack(Stack *stack) {
    int size = 0;
    while (stack[size].value || !stack[size].isOperation) {
        size += 1;
    }
    return size;
}

int CalcPostfix(Stack *stack) {
    int size = SizeStack(stack);
    int operand1, operand2, result;
    ListOperand *stackOperand = NULL;

    for (int i = 0; i < size; ++i) {
        if (!stack[i].isOperation) {
            PushOperand(&stackOperand, stack[i].value);
        } else {
            operand1 = PopOperand(&stackOperand);
            operand2 = PopOperand(&stackOperand);
            if (stack[i].value == '+') {
                result = operand2 + operand1;
            } else if (stack[i].value == '*') {
                result = operand2 * operand1;
            } else if (stack[i].value == '-') {
                result = operand2 - operand1;
            } else {
                if (operand1 == 0) {
                    FreeListOperand(&stackOperand);
                    DIVISION_ZERO
                }
                result = operand2 / operand1;
            }
            PushOperand(&stackOperand, result);
        }
    }
    result = PopOperand(&stackOperand);
    FreeListOperand(&stackOperand);

    return result;
}

int main() {
    char expression[1002];
    if (fgets(expression, 1002, stdin) == NULL) {
        return 0;
    }
    int count = 0;
    while (expression[count] != '\n') {
        count += 1;
    }
    Stack stack[count + 1];
    memset(stack, 0, count + 1);

    if (CreatePostfixNote(expression, stack) == -1) {
        return 0;
    }

    if (!stack[0].isOperation && stack[0].value != 0) {
        printf("%d", CalcPostfix(stack));
    } else {
        printf("syntax error\n");
    }

    return 0;
}
