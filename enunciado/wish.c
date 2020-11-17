#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
static char error_message[35] = "An error has occurred\n";


int cuenta(char *l);
int contar(char *l);
int encontrar(char **w,int len);
void comandos(char *l);
void seleccionar(char **w,int count,int redir);
void cambiar(char **w);
void ejecutarComando(char **w);
void agregarPath(char **w);
void ejecutarRedir(char **w,int index);
char **copiar(int start,int end,char **c);


char **pt;
int ej2,ej1 = 0;
int ptl = 1;




int main(int argc, char **argv)
{
	char *bin = "/bin";
	pt = (char **)malloc(3 * sizeof(char *));
	pt[ptl - 1] = bin;
	char *l;
	size_t len = 0;
	ssize_t lineSize = 0;
	if (argc == 1)
	{
		int seguir = 1;
	
		while (seguir == 1){
			printf("wish> ");
			lineSize = getline(&l, &len, stdin);
			comandos(l);
		}
	}
	else if (argc == 2) {
		FILE *file;
		file = fopen(argv[1], "r");
		if (file == NULL)
		{
			write(STDERR_FILENO, error_message, strlen(error_message) * sizeof(char));
			exit(1);
		}

		lineSize = getline(&l, &len, file);

		while (lineSize >= 0)
		{

			comandos(l);
			lineSize = getline(&l, &len, file);
		}
		exit(0);
	}
	else
	{
		write(STDERR_FILENO, error_message, strlen(error_message));
		exit(1);
	}
}

void comandos(char *l)
{
	char *cm;
	ej2 = 0;
	ej1 = contar(l);
	int pids[ej1];
	while ((cm = strsep(&l, "&")) != NULL)
	{
		int countWords = cuenta(cm);
		char *w[countWords];
		int length = strlen(cm);

		cm[length] = '\0';
		for (int i = 0; i < length; i++)
		{
			if (cm[i] == '\t' || cm[i] == '\n')
				cm[i] = ' ';
		}
		
		while (*cm == ' ')
			cm++;

		char *found;
		int i = 0;
		int aux = 0;
		while ((found = strsep(&cm, " ")) != NULL)
		{
			
			if (strlen(found) > 0)
			{
				aux = 1;
				w[i++] = found;
			}
		}

		if (aux == 1)
		{
		w[i] = NULL;
		int redir = encontrar(w, i);

		if(ej1>1){
			if((pids[ej2++]=fork())==0){
				seleccionar(w, countWords, redir);
				exit(0);
			}
		
		}
		else
		{
			seleccionar(w, countWords, redir);

		}
		
			
		}
	}
	int status;
	for (size_t i = 0; i < ej2; i++)
	{
		waitpid(pids[i], &status, 0);
	}

}

void seleccionar(char **w, int count, int redir)
{

	if (strcmp(w[0], "exit") == 0)
	{
		if (count > 1)
		{
			write(STDERR_FILENO, error_message, strlen(error_message));
		}
		else
			exit(0);
	}
	else if (strcmp(w[0], "cd") == 0)
	{

		cambiar(w); 
	}
	else if (strcmp(w[0], "path") == 0)
	{

		agregarPath(w);
	}
	else
	{
		if (!(*w[0]))
		{
			write(STDERR_FILENO, error_message, strlen(error_message));
		}
		else
		{
			if (redir > 0)
			{
				ejecutarRedir(w, redir);
			}

			else
				ejecutarComando(w);
		}
	}

}

void ejecutarRedir(char **w, int index)
{
	char **args = copiar(0, index, w);
	if (w[index + 1] == NULL || w[index + 2] != NULL)
	{
		write(STDERR_FILENO, error_message, strlen(error_message));
	}
	else
	{
		int fd = open(w[index + 1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		int std_out = dup(STDOUT_FILENO);
		int std_err = dup(STDERR_FILENO);
		dup2(fd, STDOUT_FILENO);
		dup2(fd, STDERR_FILENO);
		ejecutarComando(args);
		close(fd);
		dup2(std_out, STDOUT_FILENO);
		dup2(std_err, STDERR_FILENO);
	}
}

int encontrar(char **w, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (strcmp(w[i], ">\0") == 0)
		{
			return i;
		}
	}
	return 0;
}


void cambiar(char **w)
{
	if (w[1] != NULL && w[2] == NULL)
	{
		int cdSuccess = chdir(w[1]);
		if (cdSuccess == -1)
		{
			write(STDERR_FILENO, error_message, strlen(error_message));
			return;
		}
	}
	else
		write(STDERR_FILENO, error_message, strlen(error_message));
	return;
}

void ejecutarComando(char **w)
{
	int status = 1;

	for (int i = 0; i < ptl; i++)
	{

		int fullptl = strlen(pt[i]) + strlen(w[0]) + 1;
		char auxpath[fullptl];
		strcpy(auxpath, pt[i]);
		strcat(auxpath, "/");
		strcat(auxpath, w[0]);
		if (access(auxpath, X_OK) == 0)
		{

			int rc = fork();
			if (rc == 0)
			{
				if (execv(auxpath, w) == -1)
				{
					exit(1);
				}
				else
				{

					exit(0);
				}
			}
			else
			{
				wait(&status);
				if (status == 0)
				{
					break;
				}
			}
		}
		else{
			status=1;
		}
	}

	if (status == 1)
	{
		write(STDERR_FILENO, error_message, strlen(error_message));
	}
}

void agregarPath(char **w)
{
	if (pt != NULL)
		free(pt);
	pt = (char **)malloc(sizeof(char *));
	char *path_name = NULL;
	int index = 0;
	char **p = w;
	while (*(++p))
	{
		path_name = (char *)malloc(strlen(*p) * sizeof(char));
		stpcpy(path_name, *p);
		pt[index] = path_name;
		index++;
		pt = (char **)realloc(pt, (index + 1) * sizeof(char *));
	}
	pt[index] = NULL;
	ptl = index;
}

char **copiar(int start, int end, char **c)
{
	char **new_command = (char **)malloc((end - start + 1) * sizeof(char *));
	
	for (int i = 0; i < end; i++)
		new_command[i] = c[i];
	new_command[end] = NULL;
	return new_command;
}

int cuenta(char *l)
{
	int count = 1;
	int length = strlen(l);

	for (int i = 1; i < length; i++)
	{
		if (l[i] != ' ' && l[i - 1] == ' ')
		{
			count++;
		}
	}
	return count;
}

int contar(char *l)
{
	int count = 1;
	int length = strlen(l);

	for (int i = 1; i < length; i++)
	{
		if (l[i] != '&' && l[i - 1] == '&')
		{
			count++;
		}
	}
	return count;
}