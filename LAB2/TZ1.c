#include <stdio.h>

void printMatrix(int N) {
    int value = 1;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", value++);
        }
        printf("\n");
    }
}

int main() {
    int N;
    printf("Введите N: ");
    scanf("%d", &N);
    printMatrix(N);
    return 0;
}
