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

#include "commands.h"

#define REPEAT 1
#define NO_REPEAT 0

static const char CD_CMD[] = "cd";
static const char ECHO_CMD[] = "echo";
static const char EXIT_CMD[] = "exit";

static int
change_dir(char **tokens, size_t tokens_len)
{
	/* FIXME: implement */
}

static int
echo_line(char **tokens, size_t tokens_len, int *status)
{
	/* FIXME: implement */
}

int 
execute_cmd(char **tokens, size_t token_len, int *curr_status)
{
	if (token_len == 0) {
		return REPEAT;
	}

	if (strncmp(tokens[0], CD_CMD, strlen(CD_CMD))) {
		*curr_status = change_dir(tokens, tokens_len);

	} else if (strncmp(tokens[0], ECHO_CMD, strlen(ECHO_CMD))) {
		*curr_status = echo_line(tokens, tokens_len, curr_status);

	} else if (strncmp(tokens[0], EXIT_CMD, strlen(EXIT_CMD))) {
		/* terminate repeats in caller */
		return NO_REPEAT;

	} else {
		/* FIXME: add general fork/exec handling */

	}

	return REPEAT;
}
