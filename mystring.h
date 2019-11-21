#include <stdlib.h>
#include <string.h>

char * trim(char * inst){
	char * aux;
	int num_spaces = 0;
	int i;

	for (i = 0; i < strlen(inst); ++i){
		if ( inst[i] == ' '){
			num_spaces++;
		}
	}

	int size = strlen(inst) - num_spaces;

	aux = (char *) malloc(size * sizeof(char));
	memset(aux, '*', size);

	int l = 0;
	for (i = 0; i < strlen(inst); ++i){
		if ( inst[i] != ' ' ){
			aux[l] = inst[i];
			l++;
		}
	}
	aux[l] = '\0';

	return aux;
}

char * sub_string(char * inst, int inicio, int fin){
	char * aux;
	int sub_size = 0;
	int i;

	for (i = 0; i < strlen(inst); ++i){
		if ( i>=inicio && i<fin ){
			sub_size++;
		}
	}

	aux = (char *) malloc(sub_size * sizeof(char));
	memset(aux, '*', sub_size);

	int l = 0;
	for (i = 0; i < strlen(inst); ++i){
		if ( i>=inicio && i<fin ){
			aux[l] = inst[i];
			l++;
		}
	}
	aux[l] = '\0';

	return aux;
}