/*
 * string_parser.c
 *
 *  Created on: Nov 25, 2020
 *      Author: gguan, Monil
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "string_parser.h"

#define _GUN_SOURCE

int count_token(char *buf, const char *delim)
{
	// TODO：
	/*
	 *	#1.	Check for NULL string
	 *	#2.	iterate through string counting tokens
	 *		Cases to watchout for
	 *			a.	string start with delimeter
	 *			b. 	string end with delimeter
	 *			c.	account NULL for the last token
	 *	#3. return the number of token (note not number of delimeter)
	 */

	if (buf == NULL)
	{
		return 0;
	}

	char *token;
	char *ptr;
	int count = 0;

	char *copy_buf = (char *)malloc(sizeof(char) * strlen(buf) + 1);
	strcpy(copy_buf, buf);
	if (copy_buf == NULL)
	{
		return -1;
	}

	token = strtok_r(copy_buf, delim, &ptr);

	while (token != NULL)
	{
		if (strlen(token) > 0)
			count++;
		token = strtok_r(NULL, delim, &ptr);
	}

	free(copy_buf);

	return count;
}

command_line str_filler(char *buf, const char *delim)
{
	// TODO：
	/*
	 *	#1.	create command_line variable to be filled and returned
	 *	#2.	count the number of tokens with count_token function, set num_token.
	 *           one can use strtok_r to remove the \n at the end of the line.
	 *	#3. malloc memory for token array inside command_line variable
	 *			based on the number of tokens.
	 *	#4.	use function strtok_r to find out the tokens
	 *   #5. malloc each index of the array with the length of tokens,
	 *			fill command_list array with tokens, and fill last spot with NULL.
	 *	#6. return the variable.
	 */
	char *eOut;
	command_line cl;
	int j = 0;
	while (buf[j] != '\0')
	{
		if (buf[j] == '\n')
			buf[j] = '\0';
		j++;
	}
	char *copy_buf = (char *)malloc(sizeof(char) * (strlen(buf) + 1));
	strcpy(copy_buf, buf);

	cl.num_token = count_token(copy_buf, delim);
	if (cl.num_token <= 0)
	{
		cl.num_token = 0;
		return cl;
	}
	cl.command_list = (char **)malloc(sizeof(char *) * (cl.num_token + 1));

	char *ptr = NULL;
	if (cl.command_list == NULL)
	{
		eOut = "command list null\n";
		write(STDOUT_FILENO, eOut, strlen(eOut));
		cl.num_token = 0;
		return cl;
	}
	char *token = strtok_r(copy_buf, delim, &ptr);
	int index = 0;

	while (token != NULL && index < cl.num_token)
	{
		if (strlen(token) > 0)
		{
			cl.command_list[index] = strdup(token);
			if (cl.command_list[index] == NULL)
			{
				eOut = "malloc failed";
				write(STDOUT_FILENO, eOut, strlen(eOut));
				for (int k = 0; k < index; k++)
					free(cl.command_list[k]);
				free(cl.command_list);
				cl.command_list = NULL;
				cl.num_token = 0;
				return cl;
			}
			index++;
		}
		token = strtok_r(NULL, delim, &ptr);
	}
	cl.num_token = index;
	cl.command_list[index] = NULL;
	free(copy_buf);

	return cl;
}

void free_command_line(command_line *command)
{
	// TODO：
	/*
	 *	#1.	free the array base num_token
	 */

	if (command == NULL || command->command_list == NULL)
		return;

	for (int i = 0; i < command->num_token; i++)
	{
		if (command->command_list[i] != NULL)
		{
			free(command->command_list[i]);
			command->command_list[i] = NULL;
		}
	}
	free(command->command_list);
	command->command_list = NULL;
	command->num_token = 0;
}
