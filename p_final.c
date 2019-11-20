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
void exec_red(char * inst);
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
		inst = get_inst(text);

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
			break;
		}
	}

	aux = malloc((pos+1)*sizeof(char*));
	memset( aux, '*', pos+ 1);

	for ( i = 0; i < pos; i++ ){
		aux[i] = inst[i];
	}	
	aux[pos] = '\0';

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

	//se crea un proceso para ejecutar la instrucción
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

	for (i = 0; i < len; i++){
		if ( inst[i] == ' ' ){
			if ( num_args == 0 ){
				aux = i;
			}
			num_args++;
		}
	}

	//número de argumentos
	char * args [num_args+2];

	//obtención del path
	path =  malloc(sizeof(char*)*(aux+1));
	memset(path, '*', aux+1);

	for ( i= 0; i < aux; i++ ){
		path[i] = inst[i];
	}
	path[aux] = '\0';

	//obtención de argumentos
	last_e = aux;
	k = 1;

	args[0] = malloc(sizeof(char*)*size);
	memset(args[k], '*', size);
	strcpy(args[0],path);

	for ( i = (aux+1); i < len; i++ ){
		if ( inst[i] == ' '){
			current_e = i;
			size = current_e - last_e-1;
			args[k] = malloc(sizeof(char*)*size);
			memset(args[k], '*', size);
			l = 0;

			for (j = (last_e+1); j < current_e; j++){
				args[k][l] = inst[j];
				l++;
			}
			args[k][l] = '\0';

			k++;
			last_e = current_e;
		}
	}

	//se agrega el último argumento
	size = len - last_e-1;
	args[k] = malloc(sizeof(char*)*size);
	memset(args[k], '*', size);
	l = 0;
	for (j = (last_e+1); j < len; j++){
		args[k][l] = inst[j];
		l++;
	}
	args[k][l] = '\0';

	//argumento NULL
	args[num_args+1] = NULL;

	//se crea un proceso para ejecutar la instrucción
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

}

void exec_red(char * inst){

	//guardar descriptores
	int tmp_in = dup(0);
	int tmp_out = dup(1);

	//checa si hay archivo de entrada
	int fd_in;
	int has_in = 0;
	int i;
	int len = strlen(inst);
	int size;
	char * in_file;
	char * aux;

	for (i = 0; i < len; i++){
		if ( inst[i] == '<' ){
			has_in = i;
		}
	}

	if ( has_in == 0 ){
		fd_in = dup(tmp_in);
		aux = inst;
	}
	else{
		size = len - has_in - 2;
		in_file = (char *)malloc(size*sizeof(char));
		memset(in_file, '*', size);

		int l=0;
		for (i = (has_in+2); i < len; i++){
			in_file[l] = inst[i];
			l++;
		}
		in_file[i] = '\0';

		fd_in = open(in_file,O_CREAT|O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);

		size = has_in - 1;
		aux = (char *)malloc(size*sizeof(char));
		memset(aux, '*', size);

		for (i = 0; i < (has_in-1); i++){
			aux[i] = inst[i];
		}
		aux[i] = '\0';

	}

	//checar si hay redir salida
	/* aqui se saca un int con las pos de '>'
		- si hay '>' se quita la sección de > y las instrucciones
		se meten a un arreglo
		-si no hay, directo al arreglo
	*/

	//ejecución de pipes 
	int pid;
	int fd_out;



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
		exec_red(inst);
	}
}
