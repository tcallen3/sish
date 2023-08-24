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

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "parser.h"

#define SIG_COUNT 3

void
usage()
{
	fprintf(stderr, "usage: %s [-x] [-c command]\n", getprogname());
}

void
block_signals(sigset_t *block_set, sigset_t *orig_set)
{
	/* We want to ignore Ctrl-C, Ctrl-\, and Ctrl-Z */
	const int sigs[SIG_COUNT] = {SIGINT, SIGQUIT, SIGTSTP};
	size_t i;

	if (sigemptyset(orig_set) != 0) {
		perror("sigemptyset");
		exit(EXIT_FAILURE);
	}

	if (sigemptyset(block_set) != 0) {
		perror("sigemptyset");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < SIG_COUNT; i++) {
		if (sigaddset(block_set, sigs[i]) != 0) {
			perror("sigaddset");
			exit(EXIT_FAILURE);
		}
	}

	if (sigprocmask(SIG_SETMASK, block_set, orig_set) != 0) {
		perror("sigprocmask");
		exit(EXIT_FAILURE);
	}
}

int
setup_env(const char *prog)
{	
	char *full_path;
	const char *env_var = "SHELL";
	const int ok = 0;
	const int err_val = 1;
	const int overwrite = 1;
	
	full_path = realpath(prog, NULL);
	if (full_path == NULL) {
		perror("realpath");
		return err_val;
	}

	if (setenv(env_var, full_path, overwrite) != 0) {
		perror("setenv");
		(void)free(full_path);
		return err_val;
	}

	if (full_path != NULL) {
		(void)free(full_path);
	}

	return ok;
}

/*
 * A simple command line interpreter which supports a subset of Bourne
 * shell operations, including I/O redirection, pipelines, and background
 * program execution. Only a handful of builtins are supported, and no
 * effort is made to provide a useful scripting language.
 */
int 
main(int argc, char *argv[]) 
{
	extern char *optarg;
	const char *all_opts = "c:x";
	char *cmd_input = NULL;
	sigset_t block_set, orig_set;
	extern int optind;
	int cmd_echo = 0;
	int single_cmd = 0;
	int status = EXIT_SUCCESS;
	int ch;

	setprogname(argv[0]);
	block_signals(&block_set, &orig_set);
	if (setup_env(argv[0]) != 0) {
		status = EXIT_FAILURE;
		goto cleanup;
	}

	while ((ch = getopt(argc, argv, all_opts)) != -1) {
		switch (ch) {
		case 'x':
			cmd_echo = 1;
			break;
		case 'c':
			single_cmd = 1;
			cmd_input = optarg;
			break;

		case '?':
			/* FALLTHROUGH */
		default:
			usage();
			status = EXIT_FAILURE;
			goto cleanup;
			/* NOTREACHED */
		}
	}

	if (single_cmd) {
		status = parse_single(cmd_echo, cmd_input);
	} else {
		status = parse_commands(cmd_echo);
	}

cleanup:
	/* we don't really care if this fails since we're exiting */
	(void)sigprocmask(SIG_SETMASK, &orig_set, NULL);

	return status;
}
