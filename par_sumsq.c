/*
 * sumsq.c
 *
 * CS 446.646 Project 1 (Pthreads)
 *
 * Compile with --std=c99
 */

#include <pthread.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
volatile bool done = false;
volatile int num_worker_threads;

// function prototypes
void calculate_square(long number);
//function prototypes for the linked list data structure
void push(node_t * head, char act, int val);
void traverseList(node_t * head);

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

 char *fn = argv[1];
  
  node_t * head = NULL;
  head = (node_t*) malloc(sizeof(node_t));
  if (head == NULL){
  	return 1;
  }
  
  // load numbers and add them to the queue
  FILE* fin = fopen(fn, "r");
  char action;
  long num;
  while (fscanf(fin, "%c %ld\n", &action, &num) == 2) {
  	push(head, action, num);
    
  }
  fclose(fin);
  
  traverseList(head);


	// clean up and return
	return (EXIT_SUCCESS);
}

/*
 * Function to add a node to end of linked list
 */
void push(node_t * head, char act, int val) {
    node_t * current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    /*adding new variable */
    current->next = (node_t *) malloc(sizeof(node_t));
    current->next->act = act;
    current->next->val = val;
    current->next->next = NULL;
}

void traverseList(node_t * head) {
    node_t * current = head;
	int value;
	
    while (current != NULL) {
    	value = current -> val;
    	if (value){
			printf("%c ", current->act);
	        printf("%d\n", current->val);
        }
        current = current->next;
    }
}

/*
 * update global aggregate variables given a number
 */
void calculate_square(long number)
{

  // calculate the square
  long the_square = number * number;

  // ok that was not so hard, but let's pretend it was
  // simulate how hard it is to square this number!
  sleep(number);

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
}
