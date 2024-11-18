#include <stdio.h>
#include <string.h>

char *findSubstring(char *str, char *substr) {
    char *p1 = str;
    char *p2;
    char *p3;

    while (*p1) {
        p2 = p1;
        p3 = substr;

        while (*p3 && *p2 == *p3) {
            p2++;
            p3++;
        }

        if (*p3 == '\0') {
            return p1;
        }
        p1++;
    }
    return NULL;
}

int main() {
    char str[100], substr[50];

    printf("Введите строку: ");
    fgets(str, sizeof(str), stdin);
    str[strcspn(str, "\n")] = '\0'; // Удаление символа новой строки

    printf("Введите подстроку: ");
    fgets(substr, sizeof(substr), stdin);
    substr[strcspn(substr, "\n")] = '\0'; // Удаление символа новой строки

    char *result = findSubstring(str, substr);
    if (result) {
        printf("Подстрока найдена: %s\n", result);
    } else {
        printf("Подстрока не найдена\n");
    }

    return 0;
}
