#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "mssv.h"
#include "child.h"

int main(int argc, char* argv[])
{
    if((argc != 3) || (atoi(argv[2]) < 1) || (atoi(argv[2]) > 10)) /* Handle insufficient arguments and incorrect delay range */
    {
        printf("Usage: mssv <data.txt> <delay>\n");
        printf("Delay must be an integer between 1-10.\n");
        return 1; 
    }
    else /* Run program */
    {
        Validator v;
        ChildParam c[4];
        pthread_t th[4];
        int** sudoku;
        int i;
        long* id[4];
        sudoku = (int**)malloc(SIZE * sizeof(int*));
        for(i = 0; i < SIZE; i++)
        {
            sudoku[i] = (int*)malloc(SIZE * sizeof(int));
        }
        readFile(argv[1], sudoku); /* Init the sudoku 2d array */
        /* print(sudoku); */ 
        /* initialise the validator struct */
        v.sol = sudoku;
        v.col = (int*)calloc(SIZE, sizeof(int)); /* initialise with 0's */
        v.row = (int*)calloc(SIZE, sizeof(int));
        v.sub = (int*)calloc(SIZE, sizeof(int));
        v.counter = 0;
        v.sleep = atoi(argv[2]);
        v.threadsCompleted = 0;

        /* Parent thread set the parameters for the child threads */
        c[0].data = &v;
        c[0].end = 2;
        c[0].start = 0;
        c[0].n = 1;
        c[1].data = &v;
        c[1].end = 5;
        c[1].start = 3;
        c[1].n = 2;
        c[2].data = &v;
        c[2].end = 8;
        c[2].start = 6;
        c[2].n = 3;
        c[3].data = &v;
        c[3].end = 9;
        c[3].start = 0;
        c[3].n = 4;
        
        /* Allocate memory for ID's*/
        for(i = 0; i<4; i++)
        {
            id[i] = (long*)malloc(sizeof(long));
        }

        /* Init mutex and cond */
        pthread_mutex_init(&mutex, NULL);
        pthread_cond_init(&cond, NULL);
        /* Create 4 Child threads */
        for(i = 0; i < 4; i++)
        {
            if(i == 3) /* Create 4th child thread*/
            {
                if(pthread_create(&th[i], NULL, &validateCols, &c[i]) != 0) /* If value returned is not zero then there is an error otherwise create the child thread*/
                {
                    perror("Thread was unable to be created.\n");
                    return 1;
                }
            }
            else /* Create child thread 1-3 */
            {
                if(pthread_create(&th[i], NULL, &validateRows, &c[i]) != 0) /* If value returned is not zero then there is an error otherwise create the child thread*/
                {
                    perror("Thread was unable to be created.\n");
                    return 1;
                }
            }
            /* Thread should start */
        }
        
        /* Parent thread waits */
        pthread_mutex_lock(&mutex);
        while(v.threadsCompleted < 4)
        {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);

        /* Termination after validation*/
        for(i = 0; i < 4; i++)
        {
            if(pthread_join(th[i], (void**) &id[i]) != 0)
            {
                return 2;
            }
            /* Thread should finish execution */
        }

        /* Parent thread processes results */
        for(i=0;i<4;i++)
        {
            printf("Thread ID-%lu: ", (unsigned long)id[i]); /* Print ID */
            if(i == 3) /* Last thread */
            {
                printColResults(c[i].start, c[i].end, v.col);
            }
            else /* Not last thread */
            {
                printResults(c[i].start, c[i].end, v.row, v.sub);
            }
            printf("\n");
        }
        printSummary(v.counter); /* Print summary */

        /* Destroy mutex */
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);

        /* Free memory */
        for(i = 0; i < SIZE; i++)
        {
            free(sudoku[i]);
        }
        free(sudoku);
        free(v.col);
        free(v.sub);
        free(v.row);
    }
    return 0;
}

/* Prints the column results in correct format */
void printColResults(int start, int end, int* col)
{
    int i, count;
    count = 0;
    for(i = 0; i <= end; i++)
    {
        if(col[i] == 1) /*Invalid col */
        {
            if(count > 0)
            {
                printf(", ");
            }
            printf("column %d", i+1);
            count++;
        }
    }
    if(count == 0)
    {
        printf("valid");
    }
    else if(count == 1)
    {
        printf(" is invalid");
    }
    else
    {
        printf(" are invalid");
    }
}

/* Prints row/thread results in correct format */
void printResults(int start, int end, int* row, int* sub)
{
    int i, count;
    count = 0;
    for(i = start; i <= end; i++)
    {
        if(row[i] == 1) /*Invalid row */
        {
            if(count > 0)
            {
                printf(", ");
            }
            printf("row %d", i+1);
            count++;
        }
    }

    for(i = start; i <= end; i++)
    {
        if(sub[i] == 1) /*Invalid sub */
        {
            if(count > 0)
            {
                printf(", ");
            }
            printf("sub-grid %d", i+1);
            count++;
        }
    }

    if(count == 0)
    {
        printf("valid");
    }
    else if(count == 1)
    {
        printf(" is invalid");
    }
    else
    {
        printf(" are invalid");
    }
}

/* Prints the summary at the end determining validity of solution */
void printSummary(int counter)
{
    if(counter == 27)
    {
        printf("There are in total %d valid rows, columns, and sub-grids, and thus the solution is valid.\n", counter);
    }
    else
    {
        printf("There are in total %d valid rows, columns, and sub-grids, and the solution is invalid.\n", counter);
    }
}

/* Read the file and parse into 2d array */
void readFile(char* file, int** sudoku)
{
    FILE* fptr = fopen(file, "r");
    int a,b,c,d,e,f,g,h,i, nRead, nRow;
    nRow = 0;
    while ((nRead = fscanf(fptr, "%d %d %d %d %d %d %d %d %d\n", &a, &b, &c, &d, &e, &f, &g, &h, &i)) != EOF) {
        sudoku[nRow][0] = a;
        sudoku[nRow][1] = b;
        sudoku[nRow][2] = c;
        sudoku[nRow][3] = d;
        sudoku[nRow][4] = e;
        sudoku[nRow][5] = f;
        sudoku[nRow][6] = g;
        sudoku[nRow][7] = h;
        sudoku[nRow][8] = i;
        nRow++;
    }
    fclose(fptr);
}

/* Debugging */
void print(int** sudoku)
{
    int i, j;
    printf("The sudoku answers are:\n");
    for (i = 0; i < SIZE; i++)
    {
        for(j = 0; j < SIZE; j++)
        {
            printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }
}
