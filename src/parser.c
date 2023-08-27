/*

BSD 3-Clause License

Copyright (c) 2023, Thomas Allen

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <sys/param.h>

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"
#include "parser.h"

#define ECHO_PREFIX '+'

static void
print_prompt()
{
	printf("sish$ ");
}

static void
check_redirect(char *input, struct CommandInfo *cmd_info)
{
	size_t i, j, rstart, f_i;
	size_t flen;
	size_t len = strlen(input);
	const char re_in = '<';
	char fname[MAXPATHLEN];

	fname[0] = '\0';
	for (i = 0; i < len; i++) {
		if (input[i] == re_in) {
			rstart = i;
			i++;
			while (isspace(input[i]) && i < len) {
				i++;
			}

			flen = 1;
			f_i = i;
			while (!isspace(input[f_i]) && f_i < len) {
				f_i++;
				flen++;
			}
			snprintf(fname, flen, "%s", &input[i]);
			for (j = rstart; j < f_i; j++) {
				input[j] = ' ';
			}
		}
	}

	if (strlen(fname) > 0) {
		cmd_info->fd_in = open(fname, O_RDONLY);
		if (cmd_info->fd_in == -1) {
			perror(fname);
			cmd_info->fd_in = STDIN_FILENO;
		}
	}
}

static void
tokenize(char *input, struct CommandInfo *cmd_info)
{
	const char sep[] = " \t\n";
	char *curr_tok = NULL;
	size_t i = 0;

	cmd_info->tokens[i] = strtok(input, sep);
	if (cmd_info->tokens[i] == NULL) {
		cmd_info->token_count = 0;
		return;
	}
	i++;

	while ((curr_tok = strtok(NULL, sep)) != NULL &&
			i < MAX_ARGS - 1) {
		cmd_info->tokens[i] = curr_tok;
		i++;
	}
	cmd_info->tokens[i] = NULL;
	cmd_info->token_count = i;
}

int 
parse_single(int echo, char *input)
{
	struct CommandInfo cmd_info;
	int status = EXIT_SUCCESS;

	cmd_info.fd_in = STDIN_FILENO;
	cmd_info.fd_out = STDOUT_FILENO;

	if (echo) {
		printf("%c%s\n", ECHO_PREFIX, input);
	}

	check_redirect(input, &cmd_info);
	tokenize(input, &cmd_info);
	if (cmd_info.token_count == 0) {
		return status;
	}

	(void)execute_cmd(&cmd_info, &status);

	if (cmd_info.fd_in != STDIN_FILENO) {
		(void)close(cmd_info.fd_in);
	}

	if (cmd_info.fd_out != STDOUT_FILENO) {
		(void)close(cmd_info.fd_out);
	}

	return status;
}

int
parse_commands(int echo)
{
	struct CommandInfo cmd_info;
	char *input = NULL;
	size_t input_len = 0;
	int repeat = 1;
	int status = EXIT_SUCCESS;

	cmd_info.fd_in = STDIN_FILENO;
	cmd_info.fd_out = STDOUT_FILENO;

	while (repeat) {
		print_prompt();
		getline(&input, &input_len, stdin);
		if (echo) {
			printf("%c%s", ECHO_PREFIX, input);
		}

		check_redirect(input, &cmd_info);
		tokenize(input, &cmd_info);
		if (cmd_info.token_count == 0) {
			continue;
		}

		repeat = execute_cmd(&cmd_info, &status);
	}

	if (input != NULL) {
		(void)free(input);
	}

	if (cmd_info.fd_in != STDIN_FILENO) {
		(void)close(cmd_info.fd_in);
	}

	if (cmd_info.fd_out != STDOUT_FILENO) {
		(void)close(cmd_info.fd_out);
	}

	return status;
}
