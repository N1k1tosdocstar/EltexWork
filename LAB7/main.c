#include <stdio.h>
#include "calc.h"

int main() {
    int choice;
    double a, b;

    do {
        printf("\nМеню:\n");
        printf("1) Сложение\n");
        printf("2) Вычитание\n");
        printf("3) Умножение\n");
        printf("4) Деление\n");
        printf("5) Выход\n");
        printf("Введите пункт меню: ");
        scanf("%d", &choice);

        if (choice >= 1 && choice <= 4) {
            printf("Введите два числа: ");
            scanf("%lf %lf", &a, &b);
        }

        switch (choice) {
            case 1:
                printf("Результат: %g\n", add(a, b));
                break;
            case 2:
                printf("Результат: %g\n", sub(a, b));
                break;
            case 3:
                printf("Результат: %g\n", mul(a, b));
                break;
            case 4:
                printf("Результат: %g\n", div(a, b));
                break;
            case 5:
                printf("Выход из программы.\n");
                break;
            default:
                printf("Неверный пункт меню, попробуйте снова.\n");
        }
    } while (choice != 5);

    return 0;
}