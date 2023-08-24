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

#include <sys/wait.h>

#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "commands.h"

#define REPEAT 1
#define NO_REPEAT 0

#define PROC_FAILURE 127

static const char CD_CMD[] = "cd";
static const char ECHO_CMD[] = "echo";
static const char EXIT_CMD[] = "exit";

static char *
get_passwd_home()
{
	struct passwd *pass_ent;
	uid_t uid;

	uid = getuid();
	if ((pass_ent = getpwuid(uid)) == NULL) {
		perror("cd");
		return NULL;
	}

	if (pass_ent->pw_dir == NULL) {
		fprintf(stderr, "cd: no home dir defined for user\n");
		return NULL;
	}
	
	return pass_ent->pw_dir;
}

static int
change_dir(char **tokens, size_t tokens_len)
{
	char *dir = NULL;
	const char var_name[] = "HOME";
	if (tokens_len > 2) {
		fprintf(stderr, "cd: too many arguments\n");
		return 1;
	} else if (tokens_len == 2) {
		dir = tokens[1];
	} else {
		/* only cd specified, so try changing to $HOME */
		dir = getenv(var_name);

		/* $HOME isn't set, so change to user dir in getpwuid(3) */
		if (dir == NULL) {
			if ((dir = get_passwd_home()) == NULL) {
				return 1;
			}

		}
	}

	if (chdir(dir) != 0) {
		perror("cd");
		return 1;
	}

	return 0;
}

static int
echo_line(char **tokens, size_t tokens_len, int *status)
{
	char *word;
	const char stat_var[] = "$?";
	const char pid_var[] = "$$";
	pid_t pid;
	const size_t var_len = 2;
	size_t idx = 1;

	while (idx < tokens_len) {
		word = tokens[idx];
		idx++;

		if (strncmp(word, stat_var, var_len) == 0) {
			printf("%d ", *status);
			continue;
		} else if (strncmp(word, pid_var, var_len) == 0) {
			pid = getpid();
			printf("%d ", pid);
			continue;
		}

		printf("%s ", word);
	}

	printf("\n");

	return 0;
}

int 
execute_cmd(char **tokens, size_t token_len, int *curr_status)
{
	pid_t pid;

	if (token_len == 0) {
		return REPEAT;
	}

	if (strncmp(tokens[0], CD_CMD, strlen(CD_CMD))) {
		*curr_status = change_dir(tokens, token_len);

	} else if (strncmp(tokens[0], ECHO_CMD, strlen(ECHO_CMD))) {
		*curr_status = echo_line(tokens, token_len, curr_status);

	} else if (strncmp(tokens[0], EXIT_CMD, strlen(EXIT_CMD))) {
		/* terminate repeats in caller */
		return NO_REPEAT;

	} else {
		/* general fork/exec handling */
		if ((pid = fork()) == -1) {
			perror(tokens[0]);
			*curr_status = 127;
		} else if (pid == 0) {
			/* in child */
			execvp(tokens[0], tokens);
			/* should not return */
			perror(tokens[0]);
			exit(PROC_FAILURE);
		} else {
			/* in parent */
			if (waitpid(pid, curr_status, WEXITED) == -1) {
				perror(tokens[0]);
				*curr_status = PROC_FAILURE;
			}
		}
	}

	return REPEAT;
}
