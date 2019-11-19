void reset(){
	printf("\033[0m");
}

void red(){
	printf("\033[1;31m");
}

void green(){
	printf("\033[1;32m");
}

void yellow(){
	printf("\033[1;33m");
}

void blue(){
	printf("\033[1;34m");
}

void magenta(){
	printf("\033[1;35m");
}

void cyan(){
	printf("\033[1;36m");
}

void print_color(int color){
	
	switch (color){

		case 0:
			red();
		break;

		case 1:
			green();
		break;

		case 2:
			yellow();
		break;

		case 3:
			blue();
		break;

		case 4:
			magenta();
		break;

		case 5:
			cyan();
		break;

		default:
			red();
		break;
	}
}