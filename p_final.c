#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "colors.h"

#define SIZE 30

char * get_inst(char inst [SIZE]);
void exec_no_args(char * inst);
void exec_args(char * inst);
int check_syntax(char * inst);
void has_arg(char * inst);

int main(int argc, char *argv[]){
	char user[SIZE];
	char text [SIZE];
	char * inst = NULL;
	int color;
	int syn;
	
	if( argc < 2 ){
		strcpy(user,"user:~$ ");
	}
	else{
		strcpy(user,argv[1]);
		strcat(user, ":~$ ");
	}
	if ( argc < 3)
	{
		color = 0;
	}
	else{
		color = atoi(argv[2]);
	}

	strcpy(text,"START");

	while ( 1 ){

		memset(text, '*', SIZE);
		
		print_color(color);
		printf("\n%s", user);
		reset();

		fgets(text, SIZE, stdin);
		printf("text: %s\n", text);
		inst = get_inst(text);
		printf("\ninst: %s\n", inst);

		if( strcmp(inst, "FIN") == 0 ){
			break;
		}
		else{
			syn = check_syntax(inst);

			if(  syn == 0 ){
				exec_no_args(inst);
			}
			else if( syn == 1 ){
				has_arg(inst);
			}
		}
		free(inst);
		inst = NULL;
	}
}


char * get_inst(char inst[SIZE]){
	char * aux;
	int i, pos = 0;

	for (i = 0; i < SIZE; i++){
		if( inst[i] == '\n' ){
			pos = i;
			printf("Pos: %d\n", pos);
			break;
		}
	}

	aux = malloc(pos*sizeof(char*));
	memset( aux, '*', pos );
	printf("%s\n", aux);

	for ( i = 0; i < pos; i++ ){
		aux[i] = inst[i];
	}	

	printf("\nAux: %s", aux);
	return aux;
}

int check_syntax(char * inst){
	int i;
	int a = 0;
	int pos = strlen(inst);

	for (i = 0; i < pos; i++){

		if( i != (pos-1) ){
			if( inst[i] == ' ' && inst[i+1] != ' ' && i != 0 ){
				//hay argumentos
				a = 1;
			}
		}
	}

	return a;
}

void exec_no_args(char * inst){

	int result;
	int i, pos, pid;

	pid=fork();

	if( pid == -1 ){
		printf("ERROR\n");
		exit(-1);
	}
	else if( pid == 0){ 
		result = execlp(inst, inst, NULL);
		printf("Command '%s' not found\n", inst);
		exit(result);
	}
	else{ 
		wait(NULL);
	}

}

void exec_args(char * inst){
	int i, j, k, l;
	int pid, result;
	int aux = 0;
	int last_e = 0, current_e = 0, size = 0;
	int num_args = 0;;
	int len = strlen(inst);
	char * path = NULL;
	printf("(exec_args)candena: %s\n", inst);
	printf("total Len:%d\n", len);
	for (i = 0; i < len; i++){
		if ( inst[i] == ' ' ){
			if ( num_args == 0 ){
				aux = i;
			}
			num_args++;
		}
	}

	char * args [num_args+2];

	//obtención del path
	printf("Size path: %d\n",aux );
	path =  malloc(sizeof(char*)*aux);
	memset(path, '*', aux);
	printf("%s\n", path);
	for ( i= 0; i < aux; i++ ){
		path[i] = inst[i];
		printf("%c - ", path[i]);
	}

	//obtención de argumentos
	last_e = aux;
	k = 1;

	args[0] = malloc(sizeof(char*)*size);
	memset(args[k], '*', size);
	strcpy(args[0],path);

	for ( i = (aux+1); i < len; i++ ){
		if ( inst[i] == ' '){
			current_e = i;
			size = current_e - last_e - 1;
			args[k] = malloc(sizeof(char*)*size);
			memset(args[k], '*', size);
			l = 0;

			for (j = (last_e+1); j < current_e; j++){
				args[k][l] = inst[j];
				l++;
			}

			k++;
			last_e = current_e;
		}
	}

	size = len - last_e - 1;
	args[k] = malloc(sizeof(char*)*size);
	memset(args[k], '*', size);
	l = 0;
	for (j = (last_e+1); j < len; j++){
		args[k][l] = inst[j];
		l++;
	}

	args[num_args+1] = NULL;

	printf("Path: *%s*\n", path);
	printf("Argumentos\n");
	for (i = 0; i < num_args+1; i++){
		printf("*%s*\n", args[i]);
	}

	pid=fork();

	if( pid == -1 ){
		printf("ERROR\n");
		exit(-1);
	}
	else if( pid == 0){ 
		result = execvp(path, args);
		printf("Command '%s' not found\n", inst);
		exit(result);
	}
	else{ 
		wait(NULL);
	}

	//liberar memoria
	free(path);
	path = NULL;
	for (i = 0; i < num_args; i++){
		free(args[i]);
		args[i] = NULL;
	}
	printf("Se liberó la memoria");

}

void has_arg(char * inst){

	int i;
	int a = 0;
	int pos = strlen(inst);

	for (i = 0; i < pos; i++){

		if( inst[i] == '>' || inst[i] == '<' || inst[i] == '|'){
			//hay redireccionamiento e/s
			a = 1;
		}

	}

	if( a == 0){
		//Solo args  execvp
		exec_args(inst);
	}
	else{
		printf("redireccionamiento\n");
	}
}
