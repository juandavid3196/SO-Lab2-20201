#include <stdio.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



int main(int argc, char *argv[]){
	
	
	struct timeval startingTime; // tiempo de inicio
    struct timeval endingTime;// tiempo de finalizacion
	gettimeofday(&startingTime, NULL);//obtiene la hora del reloj del sistema
    int cp = fork(); // clonar proceso
    
	if (argc == 1) // si solo hay un atributo -- salir
        exit(1);
    
    else{
        
		if (cp < 0){ // si el fork retorna valor<0 -- salir
            
			fprintf(stderr, "fork failed\n");
            exit(1);
        }
        
		else if (cp== 0){// proceso hijo= para ejecutar comando 
            
			char *b[argc]; // array con espacio del total de atributos

			for (int i = 0; i < argc - 1; i++) // argc -1 porque el primero no cuenta
            	b[i] = argv[i + 1]; // pasar a el vector apartir del segundo argumento
            
           	b[argc - 1] = NULL; //el vector  debe de terminar en puntero nulo por eso b[argc] 
           	execvp(b[0], b);//ejecutar comando 
        }
        else{ 
            
			wait(NULL);// esperar ejecucion de comando
            gettimeofday(&endingTime, NULL);// retornar tiempo actual del sistema
            printf("elapsed time: %ld\n",(endingTime.tv_usec - startingTime.tv_usec)/10000); // tiempo total del proceso
        }
    }
    return 0;//salir
}