#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define LENGTH_ARRAY 100000
int rand_num_array[LENGTH_ARRAY];
int prime_array[LENGTH_ARRAY]; // Global array to store prime numbers
pthread_mutex_t mux_next_random; // Mutex for next_random
pthread_mutex_t mux_prime_array; // Mutex for prime_array access
int next_random = 0;
int prime_count = 0; // Counter for prime numbers
#define N_THREADS 4

// Function to check if a number is prime
int verify_prime(int value) {
    int divisor = 2;
    if (value < 4) {
        return 1;
    }
    while (value % divisor != 0) {
        divisor++;
    }
    if (value == divisor) {
        return 1;
    } else {
        return 0;
    }
}

void *inc_thread(void *arg) {
    int int_arg = (int)arg;

    while (1) {
        pthread_mutex_lock(&mux_next_random);
        int i = next_random;
        next_random++;
        pthread_mutex_unlock(&mux_next_random);

        if (i >= LENGTH_ARRAY) break;

        if (verify_prime(rand_num_array[i]) == 1) {
            pthread_mutex_lock(&mux_prime_array);
            prime_array[prime_count] = rand_num_array[i];
            prime_count++;
            pthread_mutex_unlock(&mux_prime_array);
        }
    }

    pthread_exit(NULL);
}

void *inc_thread_optimized(void *arg) {
    int int_arg = (int)arg;
    int chunk_size = LENGTH_ARRAY / N_THREADS;
    int start = int_arg * chunk_size;
    int end = (int_arg == (N_THREADS - 1)) ? LENGTH_ARRAY : start + chunk_size;

    for (int i = start; i < end; i++) {
        if (verify_prime(rand_num_array[i]) == 1) {
            pthread_mutex_lock(&mux_prime_array);
            prime_array[prime_count] = rand_num_array[i];
            prime_count++;
            pthread_mutex_unlock(&mux_prime_array);
        }
    }

    pthread_exit(NULL);
}

int main() {
    pthread_mutex_init(&mux_next_random, NULL);
    pthread_mutex_init(&mux_prime_array, NULL);

    for (int i = 0; i < LENGTH_ARRAY; i++) {
        rand_num_array[i] = rand() / 10;
        rand_num_array[i]=i;
    }
    ///lollo
    pthread_t t_id[N_THREADS];
    for (int i = 0; i < N_THREADS; i++) {
        pthread_create(&t_id[i], NULL, inc_thread_optimized, (void *)i);
    }

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(t_id[i], NULL);
    }

    for(int i=0;i<prime_count;i++){
        printf("%d\n", prime_array[i]);
    }

    printf("Number of prime numbers stored in prime_array: %d\n", prime_count);

    pthread_mutex_destroy(&mux_next_random);
    pthread_mutex_destroy(&mux_prime_array);
    exit(0);
}
