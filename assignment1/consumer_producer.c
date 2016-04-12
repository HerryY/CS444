#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "mt19937ar.c"

#define MAX_ELEMENTS 31


struct package {
	int number;
	int wait_time;
};

int num_elements = 0;

struct package buffer[MAX_ELEMENTS];

pthread_mutex_t mutex;

pthread_cond_t cond_produce;
pthread_cond_t cond_consume;




void* producer() 
{

	while(1) {

		//sleep before producing package
	    	sleep(genrand_int32() % 5 + 3);

		pthread_mutex_lock(&mutex);
	        
	        //if buffer is full
		while (num_elements == MAX_ELEMENTS) {
	       		printf("waiting until consumer eats something.\n");
	        	pthread_cond_wait(&cond_produce, &mutex);
	        }
	        

		struct package temp;


		//assign random numbers to struct member variables
	     
		temp.number = genrand_int32() % 100;
		temp.wait_time = genrand_int32() % 8 + 2;

		//ADD TO THE GLOBAL ARRAY HERE
		buffer[num_elements++] = temp;

		printf("Produced an item with value %d and wait time %d\n", temp.number, temp.wait_time);
	     	
		pthread_cond_signal(&cond_consume); 
	        
		pthread_mutex_unlock(&mutex);

       
        
        
    }

   
    

}

void* consumer() 
{


	while(1) {
	
		pthread_mutex_lock(&mutex);
	    
	    	//if buffer is empty
		while (num_elements == 0) {

			printf("waiting for producer to make something.\n");
			pthread_cond_wait(&cond_consume, &mutex);
	       }
	    
		pthread_mutex_unlock(&mutex);

		//sleep for appropriate time based on struct variable
		sleep((buffer[num_elements - 1]).wait_time);

		pthread_mutex_lock(&mutex);

		printf("The value in package number %d is %d.\n", num_elements - 1, (buffer[num_elements - 1]).number);

		num_elements--;

		pthread_cond_signal(&cond_produce);

		pthread_mutex_unlock(&mutex);
	    
	}

}





int main() 
{

	init_genrand(time(NULL));
	
	pthread_t produce, consume;
	
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond_consume, NULL);
	pthread_cond_init(&cond_produce, NULL);

	pthread_create(&consume, NULL, consumer, NULL);
	pthread_create(&produce, NULL, producer, NULL);

	pthread_join(consume, NULL);
	pthread_join(produce, NULL);

   
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond_consume);
	pthread_cond_destroy(&cond_produce);

}