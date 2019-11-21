#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE 50

char * get_inst(char inst [SIZE]);
void exec_no_args(char * inst);
void exec_args(char * inst);
void exec_red(char * inst);
int check_syntax(char * inst);
void has_arg(char * inst);

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
	int i, pos = 0, pid;

	for (i = 0; i < strlen(inst); i++){
		if ( inst[i] == '<' || inst[i] == '>' || inst[i] == '|'){
			pos = i;
		}
	}

	if ( pos != 0 ){
		exec_red(inst);
	}
	else{
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
	int has_in = 0, has_out = 0;
	int i;
	int len = strlen(inst);
	int size;
	int index;
	int limite;
	char * in_file;
	char * out_file;
	char * aux;

	for (i = 0; i < len; i++){
		if ( inst[i] == '<' ){
			has_in = i;

			if ( inst[i-1] == ' ' ){
				has_in = i-1;
			}
		}
		if ( inst[i] == '>' ){
			has_out = i;

			if ( inst[i-1] == ' ' ){
				has_out = i-1;
			}
		}
	}


	if ( has_in == 0 ){
		fd_in = dup(tmp_in);
		aux = inst;

		if ( has_out != 0 ){
			limite = len;
			size = len - has_out - 1;
			index = has_out;

			out_file = (char *)malloc(size*sizeof(char));
			memset(out_file, '*', size);

			int l=0;
			for (i = index; i < limite; i++){
				if ( inst[i] != '>' ){
					out_file[l] = inst[i];
					l++;
				}
			}

			out_file = trim(out_file);

			size = has_out;
			aux = (char *)malloc(size*sizeof(char));
			memset(aux, '*', size);

			for (i = 0; i < size; i++){
				aux[i] = inst[i];
			}
			aux[i] = '\0';

		}
	}
	else{

		if( has_out == 0 ){

			limite = len;
			size = len - has_in - 1;
			index = has_in;

			in_file = (char *)malloc(size*sizeof(char));
			memset(in_file, '*', size);

			int l=0;
			for (i = index; i < limite; i++){
				if ( inst[i] != '<' ){
					in_file[l] = inst[i];
					l++;
				}
			}

			in_file = trim(in_file);

			size = has_in;
			aux = (char *)malloc(size*sizeof(char));
			memset(aux, '*', size);

			for (i = 0; i < size; i++){
				aux[i] = inst[i];
			}
			aux[i] = '\0';

			fd_in = open(in_file,O_RDONLY);
			
			free(in_file);
			in_file = NULL;

		}
		else if ( has_in > has_out  && has_out != 0 ){
			limite = len;
			size = len - has_in - 1;
			index = has_in;

			in_file = (char *)malloc(size*sizeof(char));
			memset(in_file, '*', size);

			int l=0;
			for (i = index; i < limite; i++){
				if ( inst[i] != '<' ){
					in_file[l] = inst[i];
					l++;
				}
			}

			in_file = trim(in_file);

			limite = has_in;
			size = has_in - has_out - 1;
			index = has_out;

			out_file = (char *)malloc(size*sizeof(char));
			memset(out_file, '*', size);

			l=0;
			for (i = index; i < limite; i++){
				if ( inst[i] != '>' ){
					out_file[l] = inst[i];
					l++;
				}
			}

			out_file = trim(out_file);

			size = has_out;
			aux = (char *)malloc(size*sizeof(char));
			memset(aux, '*', size);

			for (i = 0; i < size; i++){
				aux[i] = inst[i];
			}
			aux[i] = '\0';
			fd_in = open(in_file,O_RDONLY);
			
			free(in_file);
			in_file = NULL;
		}
		else{

			limite = len;
			size = len - has_out - 1;
			index = has_out;

			out_file = (char *)malloc(size*sizeof(char));
			memset(out_file, '*', size);

			int l=0;
			for (i = index; i < limite; i++){
				if ( inst[i] != '>' ){
					out_file[l] = inst[i];
					l++;
				}
			}

			out_file = trim(out_file);

			limite = has_out;
			size = has_out - has_in - 1;
			index = has_in;

			in_file = (char *)malloc(size*sizeof(char));
			memset(in_file, '*', size);

			l=0;
			for (i = index; i < limite; i++){
				if ( inst[i] != '<' ){
					in_file[l] = inst[i];
					l++;
				}
			}

			in_file = trim(in_file);
			

			size = has_in;
			aux = (char *)malloc(size*sizeof(char));
			memset(aux, '*', size);

			for (i = 0; i < size; i++){
				aux[i] = inst[i];
			}
			aux[i] = '\0';

			fd_in = open(in_file,O_RDONLY);
			
			free(in_file);
			in_file = NULL;

		}
		

	}

	//Contar pipes y armar arreglo de comandos
	int num_pipes = 0;
	int num_inst = 1;
	int last_e = 0, current_e = 0;
	int k = 0, j = 0, l = 0;

	for (i = 0; i < strlen(aux); i++){
		if ( aux[i] == '|' ){
			num_pipes++;
		}
	}

	num_inst = num_inst + num_pipes;

	char * instrucciones [num_inst];

	if ( num_inst == 1 ) {

		instrucciones[0] = (char * ) malloc(strlen(aux)*sizeof(char));
		memset(instrucciones[0], '*', strlen(aux));
		l = 0;
		for (j = 0; j < strlen(aux); j++){
			instrucciones[0][l] = aux[j];
			l++;
		}
		instrucciones[0][l] = '\0';
	}
	else{

		for ( i = 0; i < strlen(aux); i++ ){
			if ( aux[i] == '|'){
				if ( aux[i-1] == ' ' ){
					current_e = i-1;
				}
				else{
					current_e = i;
				}
				size = current_e - last_e;
				instrucciones[k] = malloc(sizeof(char*)*size);
				memset(instrucciones[k], '*', size);
				l = 0;

				for (j = last_e; j < current_e; j++){
					instrucciones[k][l] = aux[j];
					l++;
				}
				instrucciones[k][l] = '\0';

				k++;

				if ( aux[current_e] == '|'){
					if ( aux[current_e+1] == ' ' ){
						last_e = current_e + 2;
					}
					else{
						last_e = current_e + 1;
					}
				}
				else if ( aux[current_e] == ' '){
					if ( aux[current_e+2] == ' ' ){
						last_e = current_e + 3;
					}
					else{
						last_e = current_e + 2;
					}
				}
			}
		}

		size = strlen(aux) - last_e;
		instrucciones[k] = malloc(sizeof(char*)*size);
		memset(instrucciones[k], '*', size);
		l = 0;
		for (j = last_e; j < strlen(aux); j++){
			instrucciones[k][l] = aux[j];
			l++;
		}
		instrucciones[k][l] = '\0';

	}

	//ejecución de pipes 
	int pid;
	int fd_out;
	int syn;

	for (i = 0; i < num_inst; i++){
		dup2(fd_in, 0);
		close(fd_in);

		if( i == num_inst - 1 ){
			//útimo comando
			if( has_out != 0 ){
				fd_out = open(out_file,O_CREAT|O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);
			}
			else{
				fd_out = dup(tmp_out);
			}
		}
		else{
			//se crean los pipes para direccionar
			int fd_pipe[2];
			pipe(fd_pipe);
			fd_out = fd_pipe[1];
			fd_in = fd_pipe[0];
		}

		dup2(fd_out, 1);
		close(fd_out);

		//se ejecuta el comando
		syn = check_syntax(instrucciones[i]);
		if(  syn == 0 ){
			exec_no_args(instrucciones[i]);
		}
		else if( syn == 1 ){
			has_arg(instrucciones[i]);
		}
	}

	dup2(tmp_in, 0);
	dup2(tmp_out, 1);
	close(tmp_in);
	close(tmp_out);

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