#include <stdio.h>

void printReverse(int arr[], int N) {
    for (int i = N - 1; i >= 0; i--) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main() {
    int N;
    printf("Введите размер массива N: ");
    scanf("%d", &N);
    
    int arr[N];
    printf("Введите элементы массива: ");
    for (int i = 0; i < N; i++) {
        scanf("%d", &arr[i]);
    }

    printf("Массив в обратном порядке: ");
    printReverse(arr, N);
    return 0;
}
