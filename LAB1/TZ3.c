#include <stdio.h>

int countOnes(unsigned int num) {
    int count = 0;
    while (num > 0) {
        count += num & 1;
        num >>= 1;
    }
    return count;
}

int main() {
    unsigned int num;
    printf("Введите целое положительное число: ");
    scanf("%u", &num);
    printf("Количество единиц в двоичном представлении: %d\n", countOnes(num));
    return 0;
}
