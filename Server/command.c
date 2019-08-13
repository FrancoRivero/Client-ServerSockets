#include "command.h"

void init_command(struct parser_element *command){
	command->background_process = 0;
	command->allow_input=0;
	command->allow_output=0;
	command->pipe_in=0;
	command->pipe_out=0;
}


/*
 * Logica de busqueda de comandos, por path absoluto, relativo o
 * en variable "PATH"
 */
void command_search_logic(struct parser_element *element){
	char *pointer = element->command;
	char *paths;
	char *path;
	char *rest;
	char paths_copy[256];
	int flag;
	switch(*pointer){
	case '/': //Camino absoluto
	case '.': //Camino relativo
		flag = execute_command(element, element->command);
		break;
	default:
			paths = getenv("PATH");//Cuidado de no modifcar esta posicion de memoria
			//Separamos los diferentes path posibles
			strcpy(paths_copy,paths);
			rest = &paths_copy;
			char *temp;
			while(*rest != NULL){
				temp = rest;
				rest = path;
				path = temp;
				strtok_r(path, ":", &rest);//separamos el comando por espacios

				char path_copy[100];
				strcpy(path_copy,path);
				strcat(path_copy,"/");
				strcat(path_copy,element->command);
				flag = execute_command(element, path_copy);
				if(flag ==0 ){//El comando se ejecuto correctamente
					return;
				}
			}
		break;

	};

	if(flag == -1){//No se encontro el comando
		printf("No such file or directory\n");
	}


}


/*
 * Ejecuta el comando como un proceso aparte, si & no fue especificado, espera por el
 */
int execute_command(struct parser_element *element, char* path)
{
	element->argv[0] = path;

	if(access(element->argv[0], X_OK) != -1){
			//Redireccionamos stdin y stdout si es necesario
			if(element->allow_input){
				switchStdin(element);
			}
			if(element->allow_output){
				switchStdout(element);
			}
			execv(element->argv[0], element->argv);
			printf("Execv error.\n");
		return 0;
	}
	else{
		//File no se encuentra en ese camino
		return -1;
	}
}

/*
 * Caso especial para cd y exit. Checkea que el elemento no sea built in, y si lo es ejecuta
 * la accion correspondiente
 */
int built_in_command(struct parser_element *element){

	if(strstr(element->command, "exit") != NULL){
		return 1; //Devolvemos verdadero
	}
	else if(strstr(element->command, "cd") != NULL)
	{
		if(element->argv[1] == NULL){
			chdir(getenv("HOME"));
		}
		else if(chdir(element->argv[1]) == -1){
			printf("cd %s No such file or directory\n", element->argv[1]);
		}
		return 1;
	}
	return 0;
}
