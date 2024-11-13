#include <stdio.h>

void printBinary(unsigned int num) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
    printf("\n");
}

int main() {
    unsigned int num;
    printf("Введите целое положительное число: ");
    scanf("%u", &num);
    printBinary(num);
    return 0;
}
