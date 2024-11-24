#include <stdio.h>
#include <string.h>

#define MAX_ABONENTS 100

// Определение структуры абонента
struct abonent {
    char name[50];
    char second_name[50];
    char tel[15];
};

// Прототипы функций
void add_abonent(struct abonent[], int*);
void delete_abonent(struct abonent[], int*);
void search_abonent(struct abonent[], int);
void display_all(struct abonent[], int);
void clear_abonent(struct abonent*);

int main() {
    struct abonent phonebook[MAX_ABONENTS] = {0};
    int count = 0; // Количество добавленных абонентов
    int choice;

    do {
        printf("\nМеню:\n");
        printf("1) Добавить абонента\n");
        printf("2) Удалить абонента\n");
        printf("3) Поиск абонентов по имени\n");
        printf("4) Вывод всех записей\n");
        printf("5) Выход\n");
        printf("Введите пункт меню: ");
        scanf("%d", &choice);
        getchar(); // Убираем символ новой строки из буфера

        switch (choice) {
            case 1:
                add_abonent(phonebook, &count);
                break;
            case 2:
                delete_abonent(phonebook, &count);
                break;
            case 3:
                search_abonent(phonebook, count);
                break;
            case 4:
                display_all(phonebook, count);
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

// Функция добавления абонента
void add_abonent(struct abonent phonebook[], int* count) {
    if (*count >= MAX_ABONENTS) {
        printf("Справочник переполнен! Нельзя добавить нового абонента.\n");
        return;
    }

    printf("Введите имя: ");
    scanf("%49s", phonebook[*count].name); // Ограничение на длину 49 символов

    printf("Введите фамилию: ");
    scanf("%49s", phonebook[*count].second_name);

    printf("Введите телефон: ");
    scanf("%14s", phonebook[*count].tel);

    getchar();

    (*count)++;
    printf("Абонент добавлен успешно.\n");
}

// Функция удаления абонента
void delete_abonent(struct abonent phonebook[], int* count) {
    char name[50];
    int i, found = 0;

    if (*count == 0) {
        printf("Справочник пуст. Удалять нечего.\n");
        return;
    }

    printf("Введите имя абонента для удаления: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    for (i = 0; i < *count; i++) {
        if (strcmp(phonebook[i].name, name) == 0) {
            clear_abonent(&phonebook[i]);
            for (int j = i; j < *count - 1; j++) {
                phonebook[j] = phonebook[j + 1];
            }
            (*count)--;
            found = 1;
            printf("Абонент %s удалён.\n", name);
            break;
        }
    }

    if (!found) {
        printf("Абонент с именем %s не найден.\n", name);
    }
}

// Функция поиска абонентов по имени
void search_abonent(struct abonent phonebook[], int count) {
    char name[50];
    int i, found = 0;

    if (count == 0) {
        printf("Справочник пуст.\n");
        return;
    }

    printf("Введите имя для поиска: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    for (i = 0; i < count; i++) {
        if (strcmp(phonebook[i].name, name) == 0) {
            printf("Имя: %s, Фамилия: %s, Телефон: %s\n",
                   phonebook[i].name, phonebook[i].second_name, phonebook[i].tel);
            found = 1;
        }
    }

    if (!found) {
        printf("Абоненты с именем %s не найдены.\n", name);
    }
}

// Функция вывода всех абонентов
void display_all(struct abonent phonebook[], int count) {
    int i;

    if (count == 0) {
        printf("Справочник пуст.\n");
        return;
    }

    printf("Список всех абонентов:\n");
    for (i = 0; i < count; i++) {
        printf("%d.\nИмя: %s\nФамилия: %s\nТелефон: %s\n\n",
               i + 1, phonebook[i].name, phonebook[i].second_name, phonebook[i].tel);
    }
}

// Функция очистки структуры
void clear_abonent(struct abonent* record) {
    memset(record, 0, sizeof(struct abonent));
}
