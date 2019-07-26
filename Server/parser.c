#include <string.h>
#include "parser.h"

/*
 * Se encargara de separar los comandos en pipes y organizar los file descriptors
 * antes de ser ejecutados.
 */
void parse_pipes(char *input_string, int *num_commands, struct parser_element *com_array){
	//Primero identificamos "|" en el input para determinar los pipes
	char *temp;
	char *rest=input_string;
	char *sentence;
	int *ppipe=NULL;
	while(*rest != NULL){
			//Logica para separar la palabra por delimiters
			temp = rest;
			rest = sentence;
			sentence = temp;
			strtok_r(sentence, "|", &rest);//separamos el comando por espacios
			struct parser_element command;
			init_command(&command); //Iinicalizo commands
			struct parser_element *command_pointer;
			command_pointer = &command;
			parse_single_command(command_pointer,sentence);//Parseamos para obtener el comando a ejecutar

			*com_array = *command_pointer;//Apunto el comando
			(*num_commands)++;
			com_array++;
	}
}


/*
 * Separa el comando en palabras mediante espacios, identifica el nombre de comando
 * y los parametros y pide su ejecucion
 */
void parse_single_command(struct parser_element *element,char *command_line){
	//Separamos el string en sus espacios
	char *rest=command_line;
	char *word;
	char *temp;
	int index=0;
	while(*rest != NULL){
		//Logica para separar la palabra por delimiters
		temp = rest;
		rest = word;
		word = temp;
		strtok_r(word, " ", &rest);//separamos el comando por espacios
		if(index == 0){//Lo primero es el comando
			element->command = word;
			element->command = strtok(element->command, " ");
		}
		else{
			if(!strcmp(word, "&")){ //Quiero poner el proceso en background
				element->background_process = 1;
				index--; //Disminuyo uno para reacomodar el indice
			}
			else if(!strcmp(word,"<")){ //Habilito archivo de entrada
				element->allow_input = 1;
				temp = rest;
				rest = word;
				word = temp;
				strtok_r(word, " ", &rest);//separamos el comando por espacios
				element->file_input = word;
				index--;
			}
			else if(!strcmp(word, ">")){ //Habilito archivo de salida
				element->allow_output = 1;
				temp = rest;
				rest = word;
				word = temp;
				strtok_r(word, " ", &rest);
				element->file_output = word;
				index--;
			}
			else{
				element->argv[index] = word; //Index 0 es guardado para el path
			}
		}
		index++;
	}
	element->argv[index] = NULL; //Necesario para delimitar el final del arreglo
}

/*
 * Cambia el control de stdin hacia un archivo
 */
void switchStdin(struct parser_element *element){
	int perm;
	perm = S_IWUSR|S_IRUSR;

	fflush(stdin);
	int new_stdin = open(element->file_input, 'r',perm);
	dup2(new_stdin,0);
	close(new_stdin);
}


/*
 * Cambia el control de stdout hacia un archivo
 */
void switchStdout(struct parser_element *element){
	int flags,perm;
	flags = O_WRONLY|O_CREAT|O_TRUNC;
	perm = S_IWUSR|S_IRUSR;

	fflush(stdout);
	int new_stdout = open(element->file_output, flags,perm);
	dup2(new_stdout,1);
	close(new_stdout);
}


void pipein(struct parser_element *element){
	int perm;
	perm = S_IWUSR|S_IRUSR;

	fflush(stdin);
	dup2(element->pipe_in,0);
}

void pipeout(struct parser_element *element){
	int flags,perm;
	flags = O_WRONLY|O_CREAT|O_TRUNC;
	perm = S_IWUSR|S_IRUSR;

	fflush(stdout);
	dup2(element->pipe_out,1);
}