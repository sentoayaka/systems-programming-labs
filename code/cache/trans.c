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

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k;
    // 定义 8 个局部变量（实验限制最多 12 个，这里用了 8 个 + 3 个循环变量）
    int v1, v2, v3, v4, v5, v6, v7, v8;

    if (N == 32 && M == 32) {
        for (i = 0; i < N; i += 8) {
            for (j = 0; j < M; j += 8) {
                for (k = i; k < i + 8; k++) {
                    // 1. 从 A 读出一整行到寄存器（局部变量）
                    // 这样即使 B 的写入踢出了 A 的缓存，A 的数据也已经安全拿到了
                    v1 = A[k][j];
                    v2 = A[k][j + 1];
                    v3 = A[k][j + 2];
                    v4 = A[k][j + 3];
                    v5 = A[k][j + 4];
                    v6 = A[k][j + 5];
                    v7 = A[k][j + 6];
                    v8 = A[k][j + 7];

                    // 2. 将数据写入 B 的对应列
                    B[j][k] = v1;
                    B[j + 1][k] = v2;
                    B[j + 2][k] = v3;
                    B[j + 3][k] = v4;
                    B[j + 4][k] = v5;
                    B[j + 5][k] = v6;
                    B[j + 6][k] = v7;
                    B[j + 7][k] = v8;
                }
            }
        }
    }

    if (N == 64 && M == 64) {
        for (i = 0; i < 64; i += 8) {
            for (j = 0; j < 64; j += 8) {
                for (k = i; k < i + 4; k++) {
                    // 1. 读取 A 上半部分：v1-v4 是左上，v5-v8 是右上
                    v1 = A[k][j]; v2 = A[k][j+1]; v3 = A[k][j+2]; v4 = A[k][j+3];
                    v5 = A[k][j+4]; v6 = A[k][j+5]; v7 = A[k][j+6]; v8 = A[k][j+7];

                    // 2. 转置 A 左上到 B 左上
                    B[j][k] = v1; B[j+1][k] = v2; B[j+2][k] = v3; B[j+3][k] = v4;

                    // 3. 【关键】将 A 右上暂时存放到 B 的右上（注意：这里不是最终位置）
                    B[j][k+4] = v5; B[j+1][k+4] = v6; B[j+2][k+4] = v7; B[j+3][k+4] = v8;
                }
                for (k = j; k < j + 4; k++) {
                    // 4. 读取 A 左下角一列 (v5-v8)
                    v5 = A[i+4][k]; v6 = A[i+5][k]; v7 = A[i+6][k]; v8 = A[i+7][k];
                    // 5. 获取 B 右上角一行暂存的数据 (v1-v4)
                    v1 = B[k][i+4]; v2 = B[k][i+5]; v3 = B[k][i+6]; v4 = B[k][i+7];

                    // 7. 将 A 左下角存入 B 右上角
                    B[k][i+4] = v5; B[k][i+5] = v6; B[k][i+6] = v7; B[k][i+7] = v8;

                    // 6. 将 B 右上角搬到 B 左下角
                    B[k+4][i] = v1; B[k+4][i+1] = v2; B[k+4][i+2] = v3; B[k+4][i+3] = v4;

                } 
                for (k = i + 4; k < i + 8; k++) {
                    // 8. 转置 A 右下角到 B 右下角
                    v1 = A[k][j+4]; v2 = A[k][j+5]; v3 = A[k][j+6]; v4 = A[k][j+7];
                    B[j+4][k] = v1; B[j+5][k] = v2; B[j+6][k] = v3; B[j+7][k] = v4;
                }
            }
        }
    }

    if (N == 67 && M == 61) {
    // 使用 16x16 分块
        for (i = 0; i < N; i += 16) {
            for (j = 0; j < M; j += 16) {
                // 在分块内部进行遍历
                // 注意：因为是不规则矩阵，必须用 k < N 和 l < M 检查边界
                for (k = i; k < i + 16 && k < N; k++) {
                    for (int l = j; l < j + 16 && l < M; l++) {
                        B[l][k] = A[k][l];
                    }
                }
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

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

