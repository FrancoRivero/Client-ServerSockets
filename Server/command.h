/*
 * command.h
 *
 *  Created on: Sep 7, 2015
 *      Author: nicolaspapp
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdlib.h>
#include "parser.h"
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void init_command(struct parser_element *);
void command_search_logic(struct parser_element *);
int execute_command(struct parser_element *element, char*);
int built_in_command(struct parser_element *element);

#endif /* COMMAND_H_ */
