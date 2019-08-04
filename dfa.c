#include <stdlib.h>
#include <stdio.h>

//#define DEBUG
#define SHOW_MACHINE

int is_accepted(int acceptStates[], int numAcceptStates, int currentState){
	int i = 0;

	if(currentState < 0){	// in dead state
		return 0;	// not accpted
	}
	
	for(i; i < numAcceptStates; ++i){
		if(acceptStates[i] == currentState){
			return 1;	// accepted
		}
	}

	return 0;	// not accepted 
}

int init_machine(FILE * fp, int ** transitions, int numStates, char * alphabet, int alphabetSize, int * acceptStates, int numAcceptStates){

	int i = 0;
	int j = 0;

	//FILE * fp = fopen(transitionFile, "r");
	

	for(i=0; i < alphabetSize; ++i){
		fscanf(fp, "%c", &alphabet[i]);
	}
	fgetc(fp);	// skip EOL
		
	for(i=0; i < numAcceptStates; ++i){
		fscanf(fp, "%d", &acceptStates[i]);
	}


	for(i=0; i < numStates; ++i){
		for(j=0; j < alphabetSize; ++j){
			fscanf(fp, "%d", &transitions[i][j]);
		}
	}

#ifdef SHOW_MACHINE 
	
	// prints alphabet
	printf("\n \u03A3 = { ");	// prints sigma
	for(i=0; i < alphabetSize; ++i){
		if(i < alphabetSize - 1){
			printf("%c, ", alphabet[i]);
		}
		else{	// dont print comma or space on last one
			printf("%c", alphabet[i]);
		}
			
	}
	printf(" }\n");
	
	// prints alphabet part of transition table
	printf("\n    \u03B4  ");	// prints delta
	for(i=0; i < alphabetSize; ++i){
		printf("%c  ", alphabet[i]);
	}
	printf("\n");


	// prints transition table
	for(i=0; i < numStates; i++){
		if(i == 0){
			printf("-> "); 
			printf("%2d ", i);
		}
		else if(is_accepted(acceptStates, numAcceptStates, i)){
			printf(" A "); 
			printf("%2d ", i);
		}
		else{			
			printf("   %2d ", i);
		}
		for(j=0; j < alphabetSize; ++j){
			printf("%2d ", transitions[i][j]);
		}
		printf("\n");
	}
	printf("\n");
#endif
	return alphabetSize;

}

int get_index(char nextChar, char alphabet[], int alphabetSize){

	int i = 0;
	
	for(i; i < alphabetSize; ++i){
		if(alphabet[i] == nextChar){
			return i;
		}
	}

	return -1; // char in not in alphabet
}

void main(void){

	char * transitionFile = "machine.txt";
	char stringFile[100] = " ";
	char * alphabet;
	int alphabetSize = 0;
	int numStates = 0;
	int numAcceptStates = 0;
	int * acceptStates;
	int  ** transitions;
	FILE *fp;
	char nextChar = ' ';
	int charIndex = 0;
	int startState = 0;
	int stringNum = 0;
	int accepted = 0;
	int rejected = 0;
	int i = 0;

	//
	// initialize machine
	//
	int currentState = startState;

	// begin machine definition
	fp = fopen(transitionFile, "r");	// open  machine definition text file
	
	if(!fp){
		printf("Cound not locate machine definition file %s.\n", transitionFile);
		exit(1);
	}

	// first line of text file is sizes for the data structures
	fscanf(fp, "%d %d %d", &numStates, &alphabetSize, &numAcceptStates);
	fgetc(fp);	// skip EOL

	// init data structures	
	alphabet = (char *)malloc(alphabetSize * sizeof(char *));
	acceptStates = (int *)malloc(numAcceptStates * sizeof(int *));
	transitions = (int **)malloc(numStates * sizeof(int *));
	for(i=0; i < numStates; ++i){
		transitions[i] = (int *)malloc(alphabetSize * sizeof(int *));
	}

	// load data from file to data structures
	init_machine(fp, transitions, numStates, alphabet, alphabetSize, acceptStates, numAcceptStates);
	fclose(fp);	// close machine definition text file	
	//	
	// machine initialized
	//
	
	printf("Enter name of test strings text file: ");
	scanf("%s", stringFile);
	printf("\n");

	fp = fopen(stringFile, "r");	// open test strings text file

	if(!fp){
		printf("\nCound not locate string file %s.\n", stringFile);
		exit(1);
	}

	nextChar = fgetc(fp); 

	while(nextChar != EOF){
	
		++stringNum;	// new string
		currentState = startState;
		printf("%d) ", stringNum);

		while(nextChar != '\n'){
			
			charIndex = get_index(nextChar, alphabet, alphabetSize);
			if(charIndex >= 0 && charIndex < alphabetSize){ // valid char 
				currentState = transitions[currentState][charIndex];

#ifdef DEBUG
				if(currentState != -1)
					printf("->%d", currentState);
#else
				printf("%c", nextChar);
#endif
				nextChar = fgetc(fp);
			}
			
			if((charIndex < 0 || charIndex > alphabetSize) || currentState == -1){	// invalid char or currentState == DEAD
#ifdef DEBUG
				printf("->dead");
#endif				
				currentState = -1;
				while(nextChar != '\n'){	// consume rest of string without transitioning, machine is DEAD!!!
#ifndef DEBUG				
					printf("%c", nextChar);
#endif
					nextChar = fgetc(fp);
				}
			}
		}
		
		if(is_accepted(acceptStates, numAcceptStates, currentState)){
			++accepted;
			printf(" -> accepted\n");
		}
		else{
			++rejected;
			printf(" -> rejected\n");
		}

		nextChar = fgetc(fp);

	}

	printf("\n");

	printf(" Conclusion of tests from %s :\n", stringFile);
	printf("	%d strings accepted\n", accepted);
	printf("	%d strings rejected\n\n", rejected);

	fclose(fp);	// close test strings text file

	return;
}
