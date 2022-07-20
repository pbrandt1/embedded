#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

enum CONSTANTS {
    NUM_THREADS = 1000,
    NUM_ITER = 1000
};

int safe = 0;
int unsafe = 0;

// Many threads will run this function
void* run(void *arg) {
    int i;
    for (i = 0; i < NUM_ITER; ++i) {
        // Increment the "unsafe" counter the normal, non-thread-safe way
        unsafe++;

        // Increment the "safe" counter atomically, to showcase safety
        __atomic_fetch_add(&safe, 1, __ATOMIC_SEQ_CST);
    }
    return NULL;
}

int main() {
    // the main function kicks off many threads, which all try to increment two counters
    // the "unsafe" counter, which is incremented unsafely in a multithreaded environment
    // and the "safe" counter, which is incremented atomically, safe for multithreading
    int i;
    pthread_t threads[NUM_THREADS];

    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, run, NULL);
    }

    for (i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    printf("safe: expected %d, was %d\n", NUM_THREADS * NUM_ITER, safe);
    assert(safe == NUM_THREADS * NUM_ITER);

    printf("unsafe: expected %d, was %d\n", NUM_THREADS * NUM_ITER, unsafe);
    assert(unsafe != NUM_THREADS * NUM_ITER);
    
    return EXIT_SUCCESS;
}
