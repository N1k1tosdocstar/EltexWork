#include <stdio.h>
#include <pthread.h>

#define COUNT 180000000
#define NUM_THREADS 4

typedef struct {
    unsigned long long local_count;
} ThreadData;

void* increment(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    data->local_count = 0;
    
    for (int i = 0; i < COUNT/NUM_THREADS; i++) {
        data->local_count++;
    }
    
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    unsigned long long total = 0;
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment, &thread_data[i]);
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        total += thread_data[i].local_count;
    }
    
    printf("Final counter: %llu\n", total);
    return 0;
}
