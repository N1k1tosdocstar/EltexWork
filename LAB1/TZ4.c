#include <stdio.h>

int main() {
    unsigned int num;
    unsigned char newByte;

    printf("Введите целое положительное число: ");
    scanf("%u", &num);

    printf("Введите новое значение для третьего байта (0-255): ");
    scanf("%hhu", &newByte);

    // Для того, чтобы обнулить третий байт и задать новое значение
    num = (num & 0xFF00FFFF) | (newByte << 16);

    printf("Новое число: %u\n", num);
    printf("Двоичное представление: ");
    for (int i = 31; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
    printf("\n");

    return 0;
}
