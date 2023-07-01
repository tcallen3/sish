# NAME

`sish` - a simple shell

# SYNOPSIS

`sish [-x] [-c command]`

# DESCRIPTION

This program implements a simple command-line interpreter based on the spec
provided by Jan Schaumann's CS 631 course, [Advanced Programming in the UNIX Environment](https://stevens.netmeister.org/631/).
While it can be used either as an interactive or login shell, it only
supports a subset of the usual shell features and makes no effort to be
useful as a scripting language. It does however support I/O redirection,
pipelines, and command backgrounding in analogy to the Bourne shell.

# OPTIONS

`sish` supports the following options:

`-c command`
Execute the given command and terminate.

`-x`
Enable tracing: write each command to stderr, preceded by '+'

# DETAILS

**Builtins**
`sish` supports several builtins which take precedence over non-builtin
commands:

`cd [dir]`
Change the current working directory. If no directory is specified, change
to the directory specified in the `HOME` environment variable. If that is
also unspecified, change to the user's home directory as determined by 
`getpwuid(3)`.

`echo [word ...]`
Print the given word(s) followed by a newline. This builtin also supports
the special values `$?` (exit status of the last command) and `$$` (current
process ID).

`exit`
Exit the current shell.

**Command Execution**
If a command sequence does not begin with a builtin, `sish` attempts to
execute it as a system command, possibly utilizing the current `PATH` 
environment variable.

**Environment**
`sish` sets the following environment variables:

`SHELL`
This is the path to the `sish` executable.

The `HOME` environment variable is also used along with `cd` as noted above.

# EXIT STATUS

`sish` returns the exit status of the last command executed, or else 127 if
the command could not be executed.

# NOTES

# KNOWN ISSUES

