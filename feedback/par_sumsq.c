/*
 * sumsq.c
 *
 * CS 446.646 Project 5 (Pthreads)
 *
 * Compile with --std=c99
 */

#include <pthread.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//defining the node and singly linked-list data type (adapted from https://www.learn-c.org/en/Linked_lists)
// defining the node data structure
typedef struct node {
    char act;
    int val;
    struct node * next;
} node_t;


// aggregate variables
volatile long sum = 0;
volatile long odd = 0;
volatile long min = INT_MAX;
volatile long max = INT_MIN;
volatile int num_worker_threads; //variable to keep track of the maximum number of threads that can be created
volatile int worker_thread_count = 0; //variable to keep track of actual number of threads created
pthread_mutex_t lock;
volatile int i = 0; //variable used for pthread id's
volatile bool busy_thread[1000]; //array to keep track of which threads are busy and which are not


// function prototypes
void calculate_square(long number);
//function prototypes for the linked list data structure
void push(node_t * head, char act, int val);

//MAIN
int main (int argc, char* argv[]){

// check and parse command line options
  if (argc != 3) {
    printf("Usage: par_sumsq <infile> <number of threads>\n");
    exit(EXIT_FAILURE);
  }
  
  //Saving number of worker threads from in-line command
  num_worker_threads = strtol(argv[2], NULL, 10);
  
  //If number of worker threads are non-positive, display error and exit
  if(num_worker_threads < 1){
  	printf("ERROR: Number of worker threads is non-positive\n");
  	exit(EXIT_FAILURE);
  }

  char *fn = argv[1]; //pointer to file for test
  
  //Creation of head node for linked list
  node_t * head = NULL;
  head = (node_t*) malloc(sizeof(node_t));
  //exit upon failed creation
  if (head == NULL){
  	exit(EXIT_FAILURE);
  }
  
  // load numbers and add them to the queue
  FILE* fin = fopen(fn, "r");
  char action;
  long num;
  while (fscanf(fin, "%c %ld\n", &action, &num) == 2) {
  	if(action == 'p' || action == 'w'){
  		push(head, action, num);
    }
    else{
    	printf("ERROR: Unrecognized action: '%c'\n", action);
    	exit(EXIT_FAILURE);
    }
  }
  fclose(fin);
  
  //initializing the mutex lock
  pthread_mutex_init(&lock, NULL);
  
  //creation of node to get task from queue
  	node_t * task = NULL;
  	node_t * node_to_delete = NULL; //node to hold node to deallocate
  	task = head;
  	int value; //value in the node
  	pthread_t thread_id[num_worker_threads]; //an array to hold pthread_id's of worker threads
  	
  	//initializing this so all threads created are initially available
  	for(int initialize = 0; initialize < num_worker_threads; ++initialize){
  		busy_thread[initialize] = 0; 
  	}
  	
  	int j = 0; //variable to use for pthread_id's; utilized to keep track of busy threads

	//traverse task queue and carrying out tasks until end of task queue	
  while(task != NULL){
  	value = task-> val; //get task value
  	if(value){ //if task value isn't zero (as no useful work would be done)
  		action = task -> act; //get action of task
  		switch (action){
    			case 'w':	sleep(value); //wait for amount of time associated with action 'w'
    						break;
    						
    			case 'p':	//if current threads is less than maximum number of threads
    						if (i < num_worker_threads){
								busy_thread[i] = 1; //set thread status as busy
    							pthread_create(&(thread_id[i]), NULL, (void*) calculate_square, (void*) value); //create a new worker thread
    							++i; //increment value for pthread id for creation of next thread
    							++worker_thread_count; //increment value of total worker threads created
    						}
    						
    						//otherwise maximum worker threads are reached and need to check for any that are available
    						else{ //if all threads are busy, then wait until not busy
    							//loop through existing threads to see which ones are available
    							//wait if all are busy
    							j = 0;
    							while(busy_thread[j]){
    								++j;
    								if(j >= worker_thread_count){
    									j = 0;	
    								}
    							}
    							//utilize the first available worker thread
    							busy_thread[j] = 1; //set thread status as busy
    							pthread_create(&(thread_id[j]), NULL, (void*) calculate_square, (void*) value); //have worker thread work on task
    							i = j;
    							++i;
    						}
    						break;
    		
    	}
  		
  	}
  	//after task is retrieved from queue and is being carried out, deallocate node to free up memory
  	node_to_delete = task;
  	task = task -> next;
  	free(node_to_delete);
  }
 	//wait until all threads complete their task before terminating
	for(int id = 0; id < worker_thread_count;++id){
		pthread_join(thread_id[id], NULL);
	}
	// print results
	printf("%ld %ld %ld %ld\n", sum, odd, min, max);
	//For debugging
	/*while(1){
		printf("%ld %ld %ld %ld\n", sum, odd, min, max);
		sleep(1);
  	}*/
	// clean up and return
	return (EXIT_SUCCESS);
}

/*
 * Function to add a node to end of linked list
 */
void push(node_t * head, char act, int val) {
    node_t * current = head;
    //traverse through list until end of list is reached
    while (current->next != NULL) {
        current = current->next;
    }

    //adding new variable to end of list
    current->next = (node_t *) malloc(sizeof(node_t));
    current->next->act = act;
    current->next->val = val;
    current->next->next = NULL;
}

/*
 * update global aggregate variables given a number
 */
void calculate_square(long number)
{
	//printf("%d\n", number); //for debugging
	//need to acquire a lock to update the global variables
	pthread_mutex_lock(&lock);
	
  // calculate the square
  long the_square = number * number;
	pthread_mutex_unlock(&lock);
	
  // ok that was not so hard, but let's pretend it was
  // simulate how hard it is to square this number!
  sleep(number);

	pthread_mutex_lock(&lock);
  // let's add this to our (global) sum
  sum += the_square;

  // now we also tabulate some (meaningless) statistics
  if (number % 2 == 1) {
    // how many of our numbers were odd?
    odd++;
  }

  // what was the smallest one we had to deal with?
  if (number < min) {
    min = number;
  }

  // and what was the biggest one?
  if (number > max) {
    max = number;
  }
  
  busy_thread[i-1] = 0; //update status so thread is no longer busy
  pthread_mutex_unlock(&lock); //thread releases lock after completion of its task
}
