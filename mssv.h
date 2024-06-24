#ifndef MSSV_H
#define MSSV_H

#define NUM_THREADS 4
#define SIZE 9
#define BUFFER_SIZE 4

typedef struct {
    int** sol;
    int* col; 
    int* row;
    int* sub; 
    int counter;
    int sleep;
    int threadsCompleted;
} Validator;

typedef struct 
{
    int start;
    int end;
    int n;
    Validator* data;
} ChildParam;

void readFile(char* file, int** sudoku);
void print(int** sudoku);
void printResults(int start, int end, int* row, int* sub);
void printColResults(int start, int end, int* col);
void printSummary(int counter);
pthread_mutex_t mutex;
pthread_cond_t cond;

#endif
