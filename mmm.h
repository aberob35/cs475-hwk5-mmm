#ifndef MMM_H_
#define MMM_H_
// globals (anything here would be shared with all threads) */
// I would declare the pointers to the matrices here (i.e., extern double **A, **B, **C),
// as well as the size of the matrices, etc.
//creation of matrices and needed global variables for threads
extern double** matrix1;
extern double** matrix2;
extern double** matrix3;
extern double** matrix4;
extern int maxSize;
extern int threadNum;
extern long *partialsums;

//struct for threads
typedef struct thread_args {
    int tid;//the given thread id
    long begin;//where to begin
    long end;//where to end
} thread_args;

void mmm_init();
void mmm_reset(double **);
void mmm_freeup();
void mmm_seq();
void *mmm_par(void *args);
double mmm_verify();
#endif /* MMM_H_ */
