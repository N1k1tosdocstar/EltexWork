#include <stdio.h>

void fillUpperTriangle(int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (j >= i) {
                printf("1 ");
            } else {
                printf("0 ");
            }
        }
        printf("\n");
    }
}

int main() {
    int N;
    printf("Введите N: ");
    scanf("%d", &N);
    fillUpperTriangle(N);
    return 0;
}
