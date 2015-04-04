#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAX_LINE 512

int fd[MAX_LINE][2];
/*
void print_cwd(){
    char curr_dir[MAX_LINE];
    getcwd(curr_dir, MAX_LINE);
    printf("%s ", curr_dir);
}*/
void print_prompt(){
    //print_cwd();
    printf("shell: ");
}

void sig_handler(int signo)
{
    	if (signo == SIGINT){
        	//fprintf(stderr, "received SIGINT\n");
    	}
}

void signal_handle(){
    	if (signal(SIGINT, sig_handler) == SIG_ERR)
       		fprintf(stderr, "\ncan't catch SIGINT\n");
}

void check_ctrld(){
    	if (feof(stdin)) {
        	printf("\n");
        	exit(0);
    	}
}

// executing one line of the input commands
void deal_command(char inputBuffer[], char *argv[], char** pipes[]){
	int run_back = 0;		// 1:  run background
	int i, j, num_pipe, k;
	char* p, **outfile, **infile;
	int infd, outfd;
	pid_t pids[MAX_LINE/2];

	i = j = num_pipe = k = 0;
	infd = outfd = -1;
	infile = outfile = NULL;

	memset(argv, 0, sizeof(argv));
	memset(fd, 0, sizeof(fd));
	memset(pids, 0, sizeof(pids));

	// parser for the command line
	// devide into several commands and arguments
	p = inputBuffer;
	while(*p == ' '|| *p == '\t' || *p == '\n' || *p == '\r') ++p;
		if (*p == '\0') return;
		if (*p == '|' || *p == '<' || *p == '>'|| *p == '&'){
		fprintf(stderr, "Invalid input. \n");
		return;
	}
	pipes[num_pipe++] = &(argv[0]);

	while(*p != '\0'){
		if (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {*p = '\0'; ++p;}
		else if (*p == '|') {
			*p = '\0';  ++p;
			argv[i++] = '\0';
			pipes[num_pipe++] = &(argv[i]);
			if (outfile != NULL){fprintf(stderr, "Invaild input >.\n"); return;}
		}else if (*p == '<'){
			if (infile != NULL || num_pipe != 1){fprintf(stderr, "Invaild input <.\n"); return;}
			*p = '\0';  ++p;
			infile = &(argv[i]);
		}else if (*p == '>'){
			if (outfile != NULL){fprintf(stderr, "Invaild input >.\n"); return;}
			*p = '\0';  ++p;
			outfile = &(argv[i]);
		}
		else if (*p == '\''){
			argv[i++] = p++;
			while (*p != '\'' && *p != '\0') ++p;
			if (*p == '\0')	{fprintf(stderr, "Invaild input \'.\n"); return;}
			else p++;
		}
		else if (*p == '\"'){
			argv[i++] = p++;
			while (*p != '\"' && *p != '\0') ++p;
			if (*p == '\0')	{fprintf(stderr, "Invaild input \'.\n"); return;}
			else p++;
		}
		else {
			argv[i++] = p;
			++p;
			while (*p != ' ' &&  *p != '\0' && *p != '|' && *p != '>' && *p != '<' && *p != '\n' && *p != '\t' && *p != '\t' && *p != '&') ++p;
		}
	}
	argv[i] = NULL;

	if (argv[i-1] != NULL)
	if (strcmp(argv[i-1], "&") == 0){
		run_back = 1;
		argv[--i] = NULL;
	}

	//executing the command "cd"
	if (strcmp(*pipes[0], "cd") == 0) {
		char* path =  (pipes[0][1]) ;
		if (chdir(path))
			fprintf(stderr, "Wrong directory name %s.\n", path);
		return;
	}

	//begining to pipe and executing the commands
	for (j = 0; j < num_pipe-1; ++j){
		pipe(fd[j]);
	}
	for (k = 0; k < num_pipe; ++k){
		pid_t pid;
		pid = fork();
		if (pid == 0){
			if (k != 0) {
				dup2  (fd[k-1][0], 0);
				close (fd[k-1][0]);
			}
			if (k != num_pipe-1){
				dup2  (fd[k][1], 1);
				close (fd[k][1]);
			}
			if (k == 0 && infile != NULL){
           			infd = open(*infile, O_RDONLY);
            			*infile = '\0';
				dup2(infd, 0);
        		}
			if (k == num_pipe - 1 && outfile != NULL){
        			outfd = open(*outfile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
        			*outfile = '\0';
				dup2(outfd, 1);
       			}

			if (execvp (*pipes[k], pipes[k]) == -1){
				fprintf(stderr, "Command not found.\n");
				exit(1);
			}
		}
		else if (pid > 0){
			pids[i] = pid;
			if (k != 0)          close(fd[k-1][0]);
			if (k != num_pipe-1) close(fd[k  ][1]);
		}
		else {fprintf(stderr, "fork error\n"); break;}
	}


	if (run_back == 0)
        	for (i = 0; i < num_pipe; ++i){
            		waitpid(pids[i], 0, 0);
        	}

	for (i = 0; i < num_pipe - 1; ++i){
        	close(fd[i][0]);
        	close(fd[i][1]);
    	}
	while (waitpid(-1, 0, WNOHANG) > 0);
}




int main(){
	char inputBuffer[MAX_LINE];
	char *argv[MAX_LINE/2];
	char **pipes[MAX_LINE/2];

	while(1){
		signal_handle();			//handle Ctrl+C , etc.
		print_prompt();
		fgets(inputBuffer, MAX_LINE, stdin);
		check_ctrld();				//Ctrl+D to exit

		deal_command(inputBuffer, argv, pipes);
	}
	return 0;
}




