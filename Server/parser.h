#ifndef PARSER_H_
#define PARSER_H_

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define STDOUT_FID 1

struct parser_element{
	char *command;//Contiene el nombre del comando
	char *argv[15];//Contiene los argumentos con los que se van a ejecutar el commando
	int background_process;
	int allow_input;
	int allow_output;
	char *file_input;
	char *file_output;
	int pipe_in; //Es el num que identifica el archivo de stdin
	int pipe_out;//Es el num que identifica el archivo de stdout
};

void parse_pipes(char*,int *, struct parser_element *);
void parse_single_command(struct parser_element *,char *);
void switchStdout(struct parser_element *);
void switchStdin(struct parser_element *);
void pipein(struct parser_element *);
void pipeout(struct parser_element *);
#endif
