#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "colors.h"
#include "mystring.h"
#include "myexec.h"

#define SIZE 50


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



