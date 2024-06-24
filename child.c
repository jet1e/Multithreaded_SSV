#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "child.h"
#include "mssv.h"

void* validateRows(void* arg) 
{
    long id;
    int** sudoku;
    int start, end, i, time;
    int x[] = {1,2,3,4,5,6,7,8,9};
    int numInRow[9];
    int sub1[9] = {0};
    int sub2[9] = {0};
    int sub3[9] = {0};
    ChildParam c = *(ChildParam*)arg;
    sudoku = c.data->sol;
    end = c.end;
    time = c.data->sleep;

    /* GET ID OF CURRENT THREAD */
    id = (unsigned long) pthread_self();
    
    /* VALIDATE ROWS */
    for(start = c.start; start <= end; start++) /* Traverse Rows */
    {
        for(i = 0; i < SIZE; i++) /* Traverse Cols */
        {
            numInRow[i] = sudoku[start][i]; /* Process row */
        }

        qsort(numInRow, SIZE, sizeof(int), compare);  /* Sort the numInRow array */

        for(i = 0; i < SIZE; i++) /* Compare the rows */
        {
            if(numInRow[i] != x[i]) /* If there is an integer that does not matchup with x, then invalid */
            {
                pthread_mutex_lock(&mutex);
                c.data->row[start] = 1;
                pthread_mutex_unlock(&mutex);
            }
        }
        if(c.data->row[start] != 1) /* If the row is not invalid then increment counter */
        {
            pthread_mutex_lock(&mutex);
            c.data->counter ++;
            pthread_mutex_unlock(&mutex);
        }
    }
    
    /* THREAD VALIDATION */
    for(start = c.start; start <= end; start++) 
    {
        for(i = 0; i < SIZE; i++)
        {
            /* Gather the sub-grid numbers and insert it in a array */
            if(i<3)
            {
                fillArray(sub1, sudoku[start][i], SIZE); /* Sugrid in column one */
            }
            else if(i>=6)
            {
                fillArray(sub3, sudoku[start][i], SIZE); /* Sugrid in column three */
            }
            else
            {
                fillArray(sub2, sudoku[start][i], SIZE); /* Sugrid in column two */
            }
        } 
    } /* Do this for each 3 threads */

    /* Sort the 3 sub grid numbers 1-9 */
    qsort(sub1, SIZE, sizeof(int), compare);
    qsort(sub2, SIZE, sizeof(int), compare);
    qsort(sub3, SIZE, sizeof(int), compare);

    for(i = 0; i < SIZE; i++) /* Compare the thread */
    {
        
        if(c.n == 1) /* Thread 1,2,3 */
        {
            if(sub1[i] != x[i])
            {
                pthread_mutex_lock(&mutex);
                c.data->sub[0] = 1;
                pthread_mutex_unlock(&mutex);
            }
            if(sub2[i] != x[i])
            {
                pthread_mutex_lock(&mutex);
                c.data->sub[1] = 1;
                pthread_mutex_unlock(&mutex);
            }
            if(sub3[i] != x[i])
            {
                pthread_mutex_lock(&mutex);
                c.data->sub[2] = 1;
                pthread_mutex_unlock(&mutex);
            }
        }
        else if(c.n == 2) /* Thread 4,5,6 */
        {
            if(sub1[i] != x[i])
            {
                pthread_mutex_lock(&mutex);
                c.data->sub[3] = 1;
                pthread_mutex_unlock(&mutex);
            }
            if(sub2[i] != x[i])
            {
                pthread_mutex_lock(&mutex);
                c.data->sub[4] = 1;
                pthread_mutex_unlock(&mutex);
            }
            if(sub3[i] != x[i])
            {
                pthread_mutex_lock(&mutex);
                c.data->sub[5] = 1;
                pthread_mutex_unlock(&mutex);
            }
        }
        else if(c.n == 3) /* Thread 7,8,9 */
        {
            if(sub1[i] != x[i])
            {
                pthread_mutex_lock(&mutex);
                c.data->sub[6] = 1;
                pthread_mutex_unlock(&mutex);
            }
            if(sub2[i] != x[i])
            {
                pthread_mutex_lock(&mutex);
                c.data->sub[7] = 1;
                pthread_mutex_unlock(&mutex);
            }
            if(sub3[i] != x[i])
            {
                pthread_mutex_lock(&mutex);
                c.data->sub[8] = 1;
                pthread_mutex_unlock(&mutex);
            }
        }
    }

    /* Increment count */
    if(c.n == 1)
    {
        for(i=0;i<3;i++)
        {
            if(c.data->sub[i] != 1)
            {
                pthread_mutex_lock(&mutex);
                c.data->counter ++;
                pthread_mutex_unlock(&mutex);
            }
        }
    }
    else if(c.n == 2)
    {
        for(i=3;i<6;i++)
        {
            if(c.data->sub[i] != 1)
            {
                pthread_mutex_lock(&mutex);
                c.data->counter ++;
                pthread_mutex_unlock(&mutex);
            }
        }
    }
    else if(c.n == 3)
    {
        for(i=6;i<9;i++)
        {
            if(c.data->sub[i] != 1)
            {
                pthread_mutex_lock(&mutex);
                c.data->counter ++;
                pthread_mutex_unlock(&mutex);
            }
        }
    }
    /* SLEEP AFTER VALIDATION */
    sleep(time);
    /* SIGNAL TO PARENT INCREASING BUFFER CONDITION */
    pthread_mutex_lock(&mutex);
    c.data->threadsCompleted++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return (void*) id; /* Return thread ID to main */
}

void* validateCols(void* arg)
{
    long id;
    int i, j, time;
    int** sudoku;
    int sub[9];
    int x[9] = {1,2,3,4,5,6,7,8,9};
    ChildParam c = *(ChildParam*)arg;
    sudoku = c.data->sol;
    time = c.data->sleep;

    /* GET ID */
    id = (unsigned long) pthread_self();

    for(j = 0; j < SIZE; j++)
    {
        /* Get the numbers of each columns and insert into an array */
        for(i = 0; i < SIZE; i++)
        {
            sub[i] = sudoku[i][j];
        }

        qsort(sub, SIZE, sizeof(int), compare); /* Sort each array*/
    
        for(i = 0; i < SIZE; i++)
        {
            if(sub[i] != x[i]) /* Check if there is an incorrect integer/duplicate if there is the col is invalid */
            {
                pthread_mutex_lock(&mutex);
                c.data->col[j] = 1;
                pthread_mutex_unlock(&mutex);
            }
        }

        if(c.data->col[j] != 1) /* Increment count if the column is valid */
        {
            pthread_mutex_lock(&mutex);
            c.data->counter ++;
            pthread_mutex_unlock(&mutex);
        }
    }
    /* Signal last thread */
    printf("Thread ID-%lu is the last thread.\n", id);
    /* SLEEP AFTER VALIDATION */
    sleep(time);
    /* SIGNAL TO PARENT INCREASING BUFFER CONDITION: THIS IS THE FINAL LAST THREAD TO WAKE UP PARENT */
    pthread_mutex_lock(&mutex);
    c.data->threadsCompleted++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return (void*) id; /* Return thread ID to main */
}

/* Fills array from start to finish. I.e. array of 0's, fill from left to right */
void fillArray(int a[], int n, int upper)
{
    int i;
    for(i = 0; i < upper; i++)
    {
        if(a[i] == 0)
        {
            a[i] = n;
            return;
        }
    }
}

/* Compare function for qsort of arrays */
/* Source: https://www.youtube.com/watch?app=desktop&v=rHoOWG6Ihs4 */
int compare(const void* x_void, const void *y_void)
{
    int x, y;
    x = *(int*)x_void;
    y = *(int*)y_void;
    return x-y;
}
