#include <stdio.h>

void printBinary(int num) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
    printf("\n");
}

int main() {
    int num;
    printf("Введите целое отрицательное число: ");
    scanf("%d", &num);
    printBinary(num);
    return 0;
}
