#include <stdio.h>

void replaceThirdByte(int *num, unsigned char newByte) {
    unsigned char *bytePointer = (unsigned char *)num;
    bytePointer[2] = newByte; // Замена третьего байта
}

int main() {
    int number;
    unsigned char newByte;

    printf("Введите целое число: ");
    scanf("%d", &number);

    printf("Введите значение для третьего байта (0-255): ");
    scanf("%hhu", &newByte);

    replaceThirdByte(&number, newByte);
    printf("Результат: %d\n", number);

    return 0;
}
