#include <stdio.h>
#include <dlfcn.h>
#include "calc.h"

int main() {
    void *handle = dlopen("./libcalc.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "Ошибка загрузки библиотеки: %s\n", dlerror());
        return 1;
    }

    int (*add)(int, int) = dlsym(handle, "add");
    int (*subtract)(int, int) = dlsym(handle, "subtract");
    int (*multiply)(int, int) = dlsym(handle, "multiply");
    int (*divide)(int, int) = dlsym(handle, "divide");

    if (!add || !subtract || !multiply || !divide) {
        fprintf(stderr, "Ошибка загрузки функций: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }

    int choice;
    int a, b;

    while (1) {
        printf("1) Сложение\n");
        printf("2) Вычитание\n");
        printf("3) Умножение\n");
        printf("4) Деление\n");
        printf("5) Выход\n");
        printf("Выберите операцию: ");
        scanf("%d", &choice);

        if (choice == 5) {
            break;
        }

        printf("Введите два числа: ");
        scanf("%d %d", &a, &b);

        switch (choice) {
            case 1:
                printf("Результат: %d\n", add(a, b));
                break;
            case 2:
                printf("Результат: %d\n", subtract(a, b));
                break;
            case 3:
                printf("Результат: %d\n", multiply(a, b));
                break;
            case 4:
                if (b == 0) {
                    printf("Ошибка: деление на ноль\n");
                } else {
                    printf("Результат: %d\n", divide(a, b));
                }
                break;
            default:
                printf("Неверный выбор\n");
        }
    }

    dlclose(handle);
    return 0;
}
