#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "rtclock.h"
#include "mmm.h"
#include <math.h>
#include<ctype.h>


double **matrix1;
double **matrix2;
double **matrix3;
double **matrix4;
int maxSize;
int threadNum;
//long *partialsums;//When I kept this in it ran it faster for some reason 
int main(int argc, char *argv[]) {
	double clockstart, clockend, clockstartSeq, clockendSeq;
	double timeHold = 0;
	double timeHold2 = 0;

	if(strcmp(argv[0],"./mmm") == 0 && argv[1] != NULL && argv[2] != NULL && argv[4] == NULL){
		//check for which mode to run and then run it 
		char* numCheck = argv[2];
		char* numCheck2 = " ";
		if(argv[3] != NULL){
		  numCheck2 = argv[3];
		}
		if(strcmp(argv[1], "S") == 0 && isdigit(numCheck[0])){
			//get necessary information from command line
			maxSize = atoi(argv[2]);
			//run first time but do not time as we want to time last 3
			mmm_init();
			mmm_seq();
			mmm_reset(matrix1);
			mmm_reset(matrix2);
			mmm_reset(matrix3);

			//runs the sequential process 3 times
			for(int i = 0; i < 3; i++){
			mmm_init();
			clockstart = rtclock(); // start clocking
			mmm_seq();
			clockend = rtclock(); // stop clocking
			timeHold += (clockend - clockstart);
			mmm_reset(matrix1);
			mmm_reset(matrix2);
			mmm_reset(matrix3);
			}

			//Freeing everything so there are no memeory leaks
			mmm_freeup();

			//Output statements 
			printf("======== \n");
			printf("mode: Sequential \n");
			printf("thread count: 1 \n");
			printf("size: %d \n", maxSize);
			printf("======== \n");
			printf("Sequential Time (avg of 3 runs): %.6f\n", timeHold/3);

		}else if(strcmp(argv[1], "P") == 0 && argv[2] < argv[3] && argv[2] != NULL && argv[3] != NULL && isdigit(numCheck[0]) && isdigit(numCheck2[0])){
			//getting needed variables
			threadNum = atoi(argv[2]);
			maxSize = atoi(argv[3]);

			//run sequential once to warmup 
			mmm_init();
			mmm_seq();
			mmm_reset(matrix1);
			mmm_reset(matrix2);
			mmm_reset(matrix3);

			//runs the sequential process 3 times to compare
			for(int i = 0; i < 3; i++){
				mmm_init();
				clockstartSeq = rtclock(); // start clocking
				mmm_seq();
				clockendSeq = rtclock(); // stop clocking
				timeHold += (clockendSeq - clockstartSeq);
				mmm_reset(matrix1);
				mmm_reset(matrix2);
				mmm_reset(matrix3);
			}
			
			
			//Running once to warm up the process
			//preparing thread arguments
			thread_args *args = (thread_args*) malloc(threadNum * sizeof(thread_args));
			for(int i = 0; i < threadNum; i++){
				args[i].tid = i;
				args[i].begin = i * maxSize / threadNum + 1;
				args[i].end = (i + 1) * maxSize / threadNum;
			}

			//allocating space to hold threads
			pthread_t *threads = (pthread_t*) malloc(threadNum * sizeof(pthread_t));
			for(int i = 0; i < threadNum; i++ ){
				pthread_create(&threads[i],NULL, mmm_par,&args[i]);
			}
			//Join Phase
			for(int i = 0; i < threadNum; i++){
				pthread_join(threads[i],NULL);
			}
			mmm_reset(matrix1);
			mmm_reset(matrix2);
			mmm_reset(matrix3);



			// Run Parallel 3 times
			for(int i = 0; i < 3; i++){
			clockstart = rtclock(); //Starts timer
			//preparing thread arguments
			//partialsums = (long*) malloc(threadNum * sizeof(long));////When I kept this in it ran it faster for some reason 
			thread_args *args = (thread_args*) malloc(threadNum * sizeof(thread_args));
			for(int i = 0; i < threadNum; i++){
				args[i].tid = i;
				args[i].begin = i * maxSize / threadNum + 1;
				args[i].end = (i + 1) * maxSize / threadNum;
			}

			//allocating space to hold threads
			pthread_t *threads = (pthread_t*) malloc(threadNum * sizeof(pthread_t));
			for(int i = 0; i < threadNum; i++ ){
				pthread_create(&threads[i],NULL, mmm_par,&args[i]);
			}

			//Join Phase
			//long finalProduct;//When I kept this in it ran it faster for some reason 
			for(int i = 0; i < threadNum; i++){
				pthread_join(threads[i],NULL);
			//	finalProduct += partialsums[i];//When I kept this in it ran it faster for some reason 
			}
			clockend = rtclock(); 
			timeHold2 += (clockend - clockstart);
			mmm_reset(matrix1);
			mmm_reset(matrix2);
			mmm_reset(matrix3);
			}
			
			//Verify step
			double diff = mmm_verify();

			//Freeing everything so there are no memeory leaks
			mmm_freeup();

			//Output statements 
			printf("======== \n");
			printf("mode: parallel \n");
			printf("thread count %d\n", threadNum);
			printf("size: %d \n", maxSize);
			printf("======== \n");
			printf("Sequential Time (avg of 3 runs): %.6f \n", timeHold / 3);
			printf("Parallel Time (avg of 3 runs): %.6f \n", timeHold2 / 3);
			printf("Speedup: %.6f \n", fabs((clockendSeq - clockstartSeq) / (clockend - clockstart)));
			printf("Verifying... largest error between parallel sequential matrix: %.6f\n", diff);

		}else{
			//Friendly Reminder of example of how to input 
			printf("Usage: ./mmm <mode> [num threads] <size> \n");
		}
	}else{
			//Friendly Reminder of example of how to input 
			printf("Usage: ./mmm <mode> [num threads] <size> \n");
	}
	return 0;
}
