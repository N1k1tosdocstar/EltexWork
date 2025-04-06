#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_STORES 5
#define NUM_CUSTOMERS 3
#define LOAD_AMOUNT 500
#define MIN_NEED 10000
#define MAX_NEED 12000

int stores[NUM_STORES];
pthread_mutex_t store_mutexes[NUM_STORES];
int customers_done = 0;
pthread_mutex_t done_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int id;
    int need;
} CustomerData;

void* customer(void* arg) {
    CustomerData* data = (CustomerData*)arg;
    
    while (data->need > 0) {
        sleep(2); // Спим перед поиском магазина
        
        for (int i = 0; i < NUM_STORES; i++) {
            if (pthread_mutex_trylock(&store_mutexes[i]) == 0) {
                printf("Customer %d woke up, need: %d\n", data->id, data->need);
                printf("Customer %d entered store %d, found %d products\n", 
                      data->id, i, stores[i]);
                
                if (stores[i] > 0) {
                    int taken = (stores[i] >= data->need) ? data->need : stores[i];
                    stores[i] -= taken;
                    data->need -= taken;
                    
                    printf("Customer %d took %d products, remaining need: %d\n",
                          data->id, taken, data->need);
                }
                
                pthread_mutex_unlock(&store_mutexes[i]);
                
                if (data->need == 0) {
                    pthread_mutex_lock(&done_mutex);
                    customers_done++;
                    pthread_mutex_unlock(&done_mutex);
                    printf("Customer %d is done shopping\n", data->id);
                    return NULL;
                }
                
                break;
            }
        }
    }
    
    return NULL;
}

void* loader(void* arg) {
    while (1) {
        // Проверяем, все ли покупатели купили
        pthread_mutex_lock(&done_mutex);
        if (customers_done == NUM_CUSTOMERS) {
            pthread_mutex_unlock(&done_mutex);
            break;
        }
        pthread_mutex_unlock(&done_mutex);
        
        // Ищем магазин для пополнения
        for (int i = 0; i < NUM_STORES; i++) {
            if (pthread_mutex_trylock(&store_mutexes[i]) == 0) {
                stores[i] += LOAD_AMOUNT;
                printf("Loader restocked store %d, now has %d products\n", 
                      i, stores[i]);
                pthread_mutex_unlock(&store_mutexes[i]);
                sleep(1); // Спим после пополнения
                break;
            }
        }
    }
    
    printf("Loader finished work\n");
    return NULL;
}

int main() {
    srand(time(NULL));
    
    // Инициализация магазинов
    for (int i = 0; i < NUM_STORES; i++) {
        stores[i] = LOAD_AMOUNT;
        pthread_mutex_init(&store_mutexes[i], NULL);
    }
    
    // Создание покупателей
    pthread_t customers[NUM_CUSTOMERS];
    CustomerData customer_data[NUM_CUSTOMERS];
    
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        customer_data[i].id = i;
        customer_data[i].need = MIN_NEED + rand() % (MAX_NEED - MIN_NEED + 1);
        pthread_create(&customers[i], NULL, customer, &customer_data[i]);
    }
    
    // Создание погрузчика
    pthread_t loader_thread;
    pthread_create(&loader_thread, NULL, loader, NULL);
    
    // Ожидание завершения покупателей
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }
    
    // Ожидание завершения погрузчика
    pthread_join(loader_thread, NULL);
    
    // Уничтожение мьютексов
    for (int i = 0; i < NUM_STORES; i++) {
        pthread_mutex_destroy(&store_mutexes[i]);
    }
    pthread_mutex_destroy(&done_mutex);
    
    return 0;
}