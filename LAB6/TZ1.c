#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Определение структуры абонента
struct abonent {
    char name[50];
    char second_name[50];
    char tel[15];
    struct abonent* prev;
    struct abonent* next;
};

// Прототипы функций
void add_abonent(struct abonent** head, struct abonent** tail);
void delete_abonent(struct abonent** head, struct abonent** tail);
void search_abonent(struct abonent* head);
void display_all(struct abonent* head);
void clear_list(struct abonent** head, struct abonent** tail);

int main() {
    struct abonent* head = NULL;
    struct abonent* tail = NULL;
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
                add_abonent(&head, &tail);
                break;
            case 2:
                delete_abonent(&head, &tail);
                break;
            case 3:
                search_abonent(head);
                break;
            case 4:
                display_all(head);
                break;
            case 5:
                printf("Выход из программы.\n");
                clear_list(&head, &tail);
                break;
            default:
                printf("Неверный пункт меню, попробуйте снова.\n");
        }
    } while (choice != 5);

    return 0;
}

// Функция добавления абонента
void add_abonent(struct abonent** head, struct abonent** tail) {
    struct abonent* new_abonent = (struct abonent*)malloc(sizeof(struct abonent));
    if (!new_abonent) {
        printf("Ошибка выделения памяти.\n");
        return;
    }

    printf("Введите имя: ");
    scanf("%49s", new_abonent->name);

    printf("Введите фамилию: ");
    scanf("%49s", new_abonent->second_name);

    printf("Введите телефон: ");
    scanf("%14s", new_abonent->tel);

    new_abonent->prev = *tail;
    new_abonent->next = NULL;

    if (*tail) {
        (*tail)->next = new_abonent;
    } else {
        *head = new_abonent;
    }

    *tail = new_abonent;

    printf("Абонент добавлен успешно.\n");
}

// Функция удаления абонента
void delete_abonent(struct abonent** head, struct abonent** tail) {
    char name[50];
    struct abonent* current = *head;

    if (!current) {
        printf("Справочник пуст. Удалять нечего.\n");
        return;
    }

    printf("Введите имя абонента для удаления: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    while (current) {
        if (strcmp(current->name, name) == 0) {
            if (current->prev) {
                current->prev->next = current->next;
            } else {
                *head = current->next;
            }

            if (current->next) {
                current->next->prev = current->prev;
            } else {
                *tail = current->prev;
            }

            free(current);
            printf("Абонент %s удалён.\n", name);
            return;
        }
        current = current->next;
    }

    printf("Абонент с именем %s не найден.\n", name);
}

// Функция поиска абонентов по имени
void search_abonent(struct abonent* head) {
    char name[50];
    struct abonent* current = head;
    int found = 0;

    if (!current) {
        printf("Справочник пуст.\n");
        return;
    }

    printf("Введите имя для поиска: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    while (current) {
        if (strcmp(current->name, name) == 0) {
            printf("Имя: %s, Фамилия: %s, Телефон: %s\n",
                   current->name, current->second_name, current->tel);
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("Абоненты с именем %s не найдены.\n", name);
    }
}

// Функция вывода всех абонентов
void display_all(struct abonent* head) {
    struct abonent* current = head;
    int i = 1;

    if (!current) {
        printf("Справочник пуст.\n");
        return;
    }

    printf("Список всех абонентов:\n");
    while (current) {
        printf("%d.\nИмя: %s\nФамилия: %s\nТелефон: %s\n\n",
               i++, current->name, current->second_name, current->tel);
        current = current->next;
    }
}

// Функция очистки списка
void clear_list(struct abonent** head, struct abonent** tail) {
    struct abonent* current = *head;
    struct abonent* next;

    while (current) {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
    *tail = NULL;
    printf("Память освобождена.\n");
}