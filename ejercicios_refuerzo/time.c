#include <stdio.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



int main(int argc, char *argv[]){
	
	
	struct timeval startingTime; 
    	struct timeval endingTime;
	gettimeofday(&startingTime, NULL);
  	  int cp = fork(); 
    
	if (argc == 1)
        exit(1);
    
	else{
        
		if (cp < 0){ 
            
		fprintf(stderr, "fork failed\n");
            exit(1);
        }
        
		else if (cp== 0){
            
		char *b[argc]; 

		for (int i = 0; i < argc - 1; i++) 
            	b[i] = argv[i + 1]; 
            
           	b[argc - 1] = NULL; 
           	execvp(b[0], b); 
        }
        else{ 
            
	wait(NULL);
        gettimeofday(&endingTime, NULL);
        printf("elapsed time: %ld\n",(endingTime.tv_usec - startingTime.tv_usec)); 
        }
    }
    return 0;
}
