#Programming assignment Shell.

### Design consideration & implementation details:

  - When the shell is ready to accept commands, it must print the prompt "shell: ".(print_prompt());
    Then user can type the command into the shell.
  - MAX_LINE 512 : Assuming that maximum length of an input line never exceeds 512 characters.

  - To exit the shell, the user must type Ctrl-D.
    Typing Ctrl-C cannot exit the shell.
  - Implemention is  in the function signal_handle() and function check_ctrld() by using the function signal(2) and feof(stdin).


  - Implementation of executing one line of the input is int the function deal_command.
  - First parse the command-line, then begin to pipe and execute the commands determined by the parser.

  - Parsing the command-line.
    Build a correct argument list for executing.
  - The shell understands the following meta-characters: '<', '>', '|', and '&'.
  - The shell also understands some escape characters, namely '\n', '\r', '\t', '\'', '\"'.

  - Begining to pipe and execute the commands.
  - Supporting cd command, which supports changing the current working directory, implemented by the system call chdir(1)
  (Something like the hints:)
  - If a valid command has been entered, the shell should fork to create a new (child) process, and the child process should exec the command, by calling execvp(2).
  - Input and output redirection by using function open, close, dup2.
  - When commands requires a pipe, create a pipe by calling pipe, and use dup2(2) to make file descriptor 0 or 1 correspond to the file descriptor of the pipe and close the file descriptors.
  - Use waitpid to wait for a program to complete execution (unless the program is in the background).
Limitations:
  - Assuming the '&' character can only appear last on a command line. Also, only the first command on the input line can have its input redirected, and only the last can have its output redirected.

  
Author: Zhen Wei
E-mail: 94hazelnut@gmail.com
