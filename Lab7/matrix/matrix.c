#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX 20
#define NUM_THREADS 10
#define ROWS_PER_THREAD (MAX / NUM_THREADS)  /* each thread handles 2 rows */

int matA[MAX][MAX]; 
int matB[MAX][MAX]; 

int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX]; 
int matProductResult[MAX][MAX]; 

/* Struct to pass thread number to each thread function */
typedef struct {
    int thread_num;  /* thread index 0 - NUM_THREADS-1 */
} ThreadArgs;

void fillMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }
}

void printMatrix(int matrix[MAX][MAX]) {
    for(int i = 0; i < MAX; i++) {
        for(int j = 0; j < MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/* Each thread computes ROWS_PER_THREAD rows of the sum matrix */
void* computeSum(void* args) {
    ThreadArgs *targs = (ThreadArgs *)args;
    int start_row = targs->thread_num * ROWS_PER_THREAD;
    int end_row   = start_row + ROWS_PER_THREAD;

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matSumResult[i][j] = matA[i][j] + matB[i][j];
        }
    }

    free(args);
    return NULL;
}

/* Each thread computes ROWS_PER_THREAD rows of the difference matrix */
void* computeDiff(void* args) {
    ThreadArgs *targs = (ThreadArgs *)args;
    int start_row = targs->thread_num * ROWS_PER_THREAD;
    int end_row   = start_row + ROWS_PER_THREAD;

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matDiffResult[i][j] = matA[i][j] - matB[i][j];
        }
    }

    free(args);
    return NULL;
}

/* Each thread computes ROWS_PER_THREAD rows of the dot product matrix */
void* computeProduct(void* args) {
    ThreadArgs *targs = (ThreadArgs *)args;
    int start_row = targs->thread_num * ROWS_PER_THREAD;
    int end_row   = start_row + ROWS_PER_THREAD;

    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j < MAX; j++) {
            matProductResult[i][j] = 0;
            for (int k = 0; k < MAX; k++) {
                matProductResult[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }

    free(args);
    return NULL;
}

int main() {
    srand(time(0));  /* Do Not Remove. Just ignore and continue below. */

    /* 1. Fill the matrices with random values */
    fillMatrix(matA);
    fillMatrix(matB);

    /* 2. Print the initial matrices */
    printf("Matrix A:\n");
    printMatrix(matA);
    printf("Matrix B:\n");
    printMatrix(matB);

    /* 3. Create pthread_t objects for our threads */
    pthread_t sum_threads[NUM_THREADS];
    pthread_t diff_threads[NUM_THREADS];
    pthread_t product_threads[NUM_THREADS];

    /* 4. Create a thread for each portion of each matrix operation */
    for (int i = 0; i < NUM_THREADS; i++)
    {
        /* Sum threads */
        ThreadArgs *sum_args = malloc(sizeof(ThreadArgs));
        sum_args->thread_num = i;
        pthread_create(&sum_threads[i], NULL, computeSum, sum_args);

        /* Diff threads */
        ThreadArgs *diff_args = malloc(sizeof(ThreadArgs));
        diff_args->thread_num = i;
        pthread_create(&diff_threads[i], NULL, computeDiff, diff_args);

        /* Product threads */
        ThreadArgs *prod_args = malloc(sizeof(ThreadArgs));
        prod_args->thread_num = i;
        pthread_create(&product_threads[i], NULL, computeProduct, prod_args);
    }

    /* 5. Wait for all threads to finish */
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(sum_threads[i],     NULL);
        pthread_join(diff_threads[i],    NULL);
        pthread_join(product_threads[i], NULL);
    }

    /* 6. Print the results */
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult);
    printf("Difference:\n");
    printMatrix(matDiffResult);
    printf("Product:\n");
    printMatrix(matProductResult);

    return 0;
}