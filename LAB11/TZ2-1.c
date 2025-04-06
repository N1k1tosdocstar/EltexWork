#include <stdio.h>

#define COUNT 180000000

int main() {
    unsigned long long counter = 0;
    
    for (unsigned long long i = 0; i < COUNT; i++) {
        counter++;
    }
    
    printf("Final counter: %llu\n", counter);
    return 0;
}
