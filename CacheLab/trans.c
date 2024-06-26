/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void transpose_32(int M, int N, int A[N][M], int B[M][N]);
void transpose_64(int M, int N, int A[N][M], int B[M][N]);
void transpose_61(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]){
    if(N == 32) transpose_32(M, N, A, B);
    else if(N == 64) transpose_64(M, N, A, B);
    else transpose_61(M, N, A, B);
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 
void transpose_32(int M, int N, int A[N][M], int B[M][N]){
    int blocking = 8;
    int a0, a1, a2, a3, a4, a5, a6, a7;
    for(int i = 0; i < N; i += blocking)
        for(int j = 0; j < M; j += blocking)
            for(int k = i; k < i + blocking; k++){
                a0 = A[k][j];
                a1 = A[k][j + 1];
                a2 = A[k][j + 2];
                a3 = A[k][j + 3];
                a4 = A[k][j + 4];
                a5 = A[k][j + 5];
                a6 = A[k][j + 6];
                a7 = A[k][j + 7];

                B[j][k] = a0;
                B[j + 1][k] = a1;
                B[j + 2][k] = a2;
                B[j + 3][k] = a3;
                B[j + 4][k] = a4;
                B[j + 5][k] = a5;
                B[j + 6][k] = a6;
                B[j + 7][k] = a7;
            }
}

void transpose_64(int M, int N, int A[N][M], int B[M][N]){
    int blocking = 4;
    int a0, a1, a2, a3;
    for(int i = 0; i < N; i += blocking)
        for(int j = 0; j < M; j += blocking)
            if(i != j)
                for(int k = i; k < i + blocking; k++){
                    a0 = A[k][j];
                    a1 = A[k][j + 1];
                    a2 = A[k][j + 2];
                    a3 = A[k][j + 3];
                    B[j][k] = a0;
                    B[j + 1][k] = a1;
                    B[j + 2][k] = a2;
                    B[j + 3][k] = a3;
                }
    for(int i = 0; i < N;i += blocking)
        for(int j = i; j < i + blocking; j++){
            a0 = A[j][i];
            a1 = A[j][i + 1];
            a2 = A[j][i + 2];
            a3 = A[j][i + 3];
            B[i][j] = a0;
            B[i + 1][j] = a1;
            B[i + 2][j] = a2;
            B[i + 3][j] = a3;
        }
}

void transpose_61(int M, int N, int A[N][M], int B[M][N]){
    int blocking = 8;
    int a0 = 0, a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0, a6 = 0, a7 = 0;
    for(int i = 0; i < N; i += blocking)
        for(int j = 0; j < M; j += blocking)
            for(int k = i; k < i + blocking && k < N; k++){
                a0 = A[k][j];
                if(j + 1 < M) a1 = A[k][j + 1];
                if(j + 2 < M) a2 = A[k][j + 2];
                if(j + 3 < M) a3 = A[k][j + 3];
                if(j + 4 < M) a4 = A[k][j + 4];
                if(j + 5 < M) a5 = A[k][j + 5];
                if(j + 6 < M) a6 = A[k][j + 6];
                if(j + 7 < M) a7 = A[k][j + 7];

                B[j][k] = a0;
                if(j + 1 < M) B[j + 1][k] = a1;
                if(j + 2 < M) B[j + 2][k] = a2;
                if(j + 3 < M) B[j + 3][k] = a3;
                if(j + 4 < M) B[j + 4][k] = a4;
                if(j + 5 < M) B[j + 5][k] = a5;
                if(j + 6 < M) B[j + 6][k] = a6;
                if(j + 7 < M) B[j + 7][k] = a7;
            }
}
/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

