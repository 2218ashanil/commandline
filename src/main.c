#include "shell.h"
#include "debug.h"
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

/*
 * As in previous hws the main function must be in its own file!
 */
int alrmarg = 0;
char* alarmstring;
int wholestring;
int alarmlength;
volatile sig_atomic_t printstring = 0;


void sigalrmhandler(int sig){
	//int olderrno = errno;
	//signal(SIGALRM, SIG_IGN);
	// char string[256];
	// sprintf(string, "%d", alrmarg);
	write(STDOUT_FILENO, "Your ", 5);
	write(STDOUT_FILENO, alarmstring, alarmlength);
	// //printf("Statement: %s\n", sigalarmstring);
	write(STDOUT_FILENO, " second timer is up.\n", 21);
	//printstring = 1;
	//printf("Your %d second timer is up.\n", alrmarg);
	//exit(0);
	//fflush(stdout);
	rl_forced_update_display();
}

void sigusr2handler(int sig){
	//int olderrno = errno;
	//signal(SIGALRM, SIG_IGN);
	char* string = "Well that was easy.\n";
	write(STDOUT_FILENO, string, stringlength(string));
	//fflush(stdout);
	//rl_forced_update_display();
}

void sigchldhandler(int sig, siginfo_t * info, void* var){
	long cpuTime = ((info->si_utime + info->si_stime)/ sysconf(_SC_CLK_TCK));
	// char a[1024];
	// char b[1024];
	// char* pidstring = inttocharacter((int) getpid(), b);
 //    char* cpustring = inttocharacter(cpuTime, a);
 //    int cpulength = stringlength(alarmstring);
	int pid = getpid();
	// write(STDOUT_FILENO, "Child with PID ", 15);
	// write(STDOUT_FILENO, pidstring, stringlength(pidstring));
	// write(STDOUT_FILENO, " has died. It spent ", 20);
	// write(STDOUT_FILENO, cpustring, cpulength);
	// write(STDOUT_FILENO, " milliseconds utilizing the CPU.\n", 33);
	printf("Child with PID %i has died. It spent %i milliseconds utilizing the CPU.\n", pid, (int) cpuTime);
}

int main(int argc, char const *argv[], char* envp[]){
    /* DO NOT MODIFY THIS. If you do you will get a ZERO. */
    rl_catch_signals = 0;
    /* This is disable readline's default signal handlers, since you are going to install your own.*/
    sigset_t blockedset;
    sigemptyset(&blockedset);
    sigaddset(&blockedset, SIGTSTP);
    sigprocmask(SIG_BLOCK, &blockedset, NULL);

    struct sigaction childaction;
    //initialize the flags
    childaction.sa_flags =SA_SIGINFO;
    sigemptyset(&childaction.sa_mask);
    childaction.sa_sigaction = &sigchldhandler;
    sigaction(SIGCHLD, & childaction, NULL);
    char *cmd;
    //char pwd[256];
    char *pwd = malloc(256);
    size_t size = 256;
    while((getcwd(pwd, size) == NULL) && (errno == ERANGE)){
        size = size *2;
    }
    char* prevDir = NULL;
    char* read = malloc(15 + strlen(pwd));
    char* temp = "<aanil> : <";
    strcpy(read, temp);
    strcat(read, pwd);
    char* cat = "> $ ";
    strcat(read, cat);
    signal(SIGUSR2, sigusr2handler);
    signal(SIGALRM, sigalrmhandler);
    //printf("PID %d\n", (int)getpid());
    //"<aanil> : <pwd> $ "
    //char space = ' ';
    while((cmd = readline(read)) != NULL) {//FIX THIS LINE TO INCLUDE FILEPATH
         //char* temp =cmd;
         // char* formCmd = strsep(&temp, &space);
         // if(strcmp(formCmd, "") == 0){
         //    formCmd = strsep(&temp, &space);
         //    formCmd = strsep(&temp, &space);
         // }
         // if(strcmp(formCmd, "") == 0){
         //    formCmd = strsep(&temp, &space);
        // }
        int found =0;
        char* formCmd = checkCMD(cmd);
        char* compare = malloc(strlen(cmd));
        char* temp = compare;
        for(int i =0; i< strlen(cmd); i++){
            *temp = '*';
            temp++;
        }
        *temp = '\0';

        if(formCmd != NULL){
            if(strcmp(formCmd, compare) !=0){
            	char* integer = cmd;
        	alrmarg = atoi(integer);
        	if(alrmarg != 0){
        		char a[1024];
        		//char* numberstring;
        		 alarmstring = inttocharacter(alrmarg, a);
        		 alarmlength = strlen(alarmstring);
        		 alarm(alrmarg);
        		 // while(1){
        		 // 	;
        		 // }
        		 //exit(0);
        		 found =1;
        		 //exit(0);
        	}
                if(strcmp(formCmd, "help") == 0){
	                fprintf(stderr, "%s\n",
	                "help: Print a list of all builtins and their basic usage in a single column.\n"
	                "exit: Exits the shell by using the exit(3) function.\n"
	                "cd: Changes the current working directory of the shell by using the chdir(2) system call.\n"
	                "\tcd - should change the working directory to the last directory the user was in.\n"
	                "\tcd with no arguments should go to the user's home directory which is stored in the HOME environment variable.\n"
	                "\tcd.. moves up one directory.\n"
	                "\tcd. stays in the directory you are in currently.\n"
	                "pwd: prints the absolute path of the current working directory, which can be obtained by using the getcwd(3) function.\n");
	                found =1;
            	}
            else if (strcmp(formCmd, "pwd")==0){
                int status;
                //pid_t parentproc =getpid();
                pid_t pid = fork();
                if(pid == -1){//error in forking
                    fprintf(stderr,"Fork error" );
                }
                else if(pid == 0){//this is the child process
                    char cwd[size];
                    while((getcwd(cwd, size) == NULL) && (errno == ERANGE)){
                        size = size *2;
                    }
                    if((getcwd(cwd, size) == NULL)){
                        perror("error in getcwd()");
                    }
                    if(getcwd(cwd, size) != NULL){
                        found =1;
                        printf("%s\n", cwd);
                    }
                    exit(0);
                }
                else{
                    waitpid(-1, &status, 0);
                }
                found =1;
            }
            //starts with cd
            else if(strcmp(formCmd, "cd -") == 0){//change working directory to the last directory it was in
                //char* prevDir = getenv("OLDPWD");
                //chdir("OLDPWD");
                if(prevDir == NULL){
                    perror("bash: cd: OLDPWD not set");//IS THIS THE CORRECT WAY TO SHOW AN ERROR
                }
                else{
                    //char* tempDir = prevDir;
                    int num = 0;
                    while(*prevDir != '\0'){
                        num++;
                        prevDir++;
                    }
                    prevDir = prevDir - num;
                    char *temp = malloc(strlen(prevDir));
                    strcpy(temp, prevDir);
                    while(((prevDir = getcwd(pwd, size)) == NULL) && (errno == ERANGE)){
                        size = size *2;
                    }
                    if(((prevDir = getcwd(pwd, size)) == NULL)){
                        perror("error in getcwd()");
                    }
                    if(getcwd(pwd, sizeof(pwd)) != NULL){
                        //printf("%s\n", pwd);
                    }
                    chdir(temp);
                    setenv("PWD", temp, 1);
                    // char cwd[size];
                    // while((getcwd(cwd, size) == NULL) && (errno == ERANGE)){
                    //     size = size *2;
                    // }
                    // if((getcwd(cwd, size) == NULL)){
                    //     perror("error in getcwd()");
                    // }
                    // if(getcwd(cwd, size) != NULL){
                    //     setenv("PWD", cwd, 1);
                    //     printf("%s\n", cwd);
                    // }

                    free(temp);

                }
                // while((getcwd(pwd, size) == NULL) && (errno == ERANGE)){
                //     size = size *2;
                // }
                // if((getcwd(pwd, size) == NULL)){
                //     perror("error in getcwd()");
                // }
                // if(getcwd(pwd, sizeof(pwd)) != NULL){
                //     printf("%s\n", pwd);
                // }
                found =1;
                //printf("Oldpwd");
            }

            else if(strcmp(formCmd, "cd") == 0){
                while(((prevDir = getcwd(pwd, size)) == NULL) && (errno == ERANGE)){
                    size = size *2;
                }
                if(((prevDir = getcwd(pwd, size)) == NULL)){
                    perror("error in getcwd()");
                }
                //prevDir = getcwd(pwd, size);
                chdir(getenv("HOME"));
                char cwd[size];
                //printf("%s\n", getcwd(cwd, size));
                setenv("PWD", cwd, 1);
                found =1;
                //printf("home");
            }
            else if(strcmp(formCmd, "cd .") == 0){
                char cwd[size];
                //printf("%s\n", getcwd(cwd, size));
                chdir(getcwd(cwd, size));
                setenv("PWD", cwd, 1);
                //int i =0;
                //printf("%s\n", getcwd(pwd, size));
                // char cwd[256];
                // getcwd(cwd, 256);
                // //printf("\ncd. ");
                // i =0;
                // while(cwd[i] != '\0'){
                //     printf("%c", cwd[i]);
                //     i++;
                // }
                found =1;
            }
            else if(strcmp(formCmd, "cd ..") == 0){
                //int i =0;
                // printf("Before change: \n");
                // while(pwd[i] != '\0'){
                //     printf("%c", pwd[i]);
                //     i++;
                // }
                //printf(getcwd())
                while(((prevDir = getcwd(pwd, size)) == NULL) && (errno == ERANGE)){
                    size = size *2;
                }
                if(((prevDir = getcwd(pwd, size)) == NULL)){
                    perror("error in getcwd()");
                }
                chdir("../");
                // if(getcwd(pwd, sizeof(pwd)) != NULL){
                //     printf("%s\n", pwd);
                // }
                found =1;
                char cwd[size];
                while((getcwd(cwd, size) == NULL) && (errno == ERANGE)){
                    size = size *2;
                }
                if((getcwd(cwd, size) == NULL)){
                    perror("error in getcwd()");
                }
                if(getcwd(cwd, size) != NULL){
                    setenv("PWD", cwd, 1);
                    //printf("%s\n", cwd);
                }
            }
            else if(strncmp(formCmd, "cd ", 3) == 0){
                //TODO
                char* tempDir = NULL;
                formCmd = formCmd +3;
                while(((tempDir = getcwd(pwd, size)) == NULL) && (errno == ERANGE)){
                    size = size *2;
                }
                if(((tempDir = getcwd(pwd, size)) == NULL)){
                    perror("error in getcwd()");
                }
                if(*formCmd == '/'){
                    if(chdir(formCmd) == -1){
                        printf("bash: cd: %s: No such file or directory\n", formCmd);
                    }
                    else{
                        if(tempDir != NULL)
                        prevDir = tempDir;
                        setenv("PWD",formCmd,1);
                    }
                }
                else{
                    char* anotherDir = NULL;
                    char* cwd = malloc(size);
                    if((anotherDir = getcwd(cwd, size)) != NULL){
                        strcat(anotherDir, "/");
                        strcat(anotherDir, formCmd);
                        if(chdir(anotherDir) == -1){
                            fprintf(stderr,"bash: cd: %s: No such file or directory\n", formCmd);
                        }
                        else{
                            if(tempDir != NULL){
                                prevDir = tempDir;
                                setenv("PWD",anotherDir,1);
                                //printf("After cd %s\n", getenv("PWD"));
                            }
                        }
                    }
                    free(cwd);
                }
                formCmd = formCmd -3;
                found =1;
                //printf("cd with arguments");
            }
            else if (strcmp(formCmd, "exit")==0){
                found =1;
                exit(3);
            }
            else{
                //
            }
        }
        if((formCmd != NULL)){
            if((found !=1)){
            //Part 2
                // int status;
                // ////pid_t parentproc =getpid();
                // pid_t pid = fork();
                int input = -1;
                int output = -1;
                // if(pid == -1){//error in forking
                //     perror("Fork error");
                // }
                // else if(pid == 0){//this is the child process
                    char* line = cmd;
                    int space = -1;
                    char temp[size];
                    int count = 0;
                    int numargs =0;
                    while(*line != '\0'){
                        while(*line == ' '){
                            line++;
                        }
                        if(space != -1 && (*line != '\0')){
                            space = 1;
                            temp[count] = ' ';
                            count++;
                        }
                        while((*line != '\0') && (*line != ' ')){
                            temp[count] = *line;
                            count++;
                            line++;
                            space = 1;
                            if( (*line == ' ') || (*line == '\0')){
                                numargs++;
                            }
                        }
                    }
                    temp[count] = '\0';
                    // char buf[count];
                    // for(int i =0; i< count; i++){
                    //     buf[i] = temp[i];
                    // }
                    char* buf = malloc(count);
                    strcpy(buf, temp);
                    //int charsInEachString[numargs -1];//dont includethe null terminated string accounted by numargs
                    //findCharsEachString(charsInEachString, buf, count);
                    //printf("Buffer: %s", buf);
                    char *token = strtok(buf, " ");
                    numargs++;
                    char* args[numargs];
                    int argcount = numargs;
                    int i =0;
                    while(token != NULL){
                        args[i] = token;
                        token =strtok(NULL, " ");
                        i++;
                    }
                        args[i] = (char*) 0;
                    //}
                    // for(i=0; i<numargs; i++){
                    //     printf("Args: %s\n", args[i]);
                    // }
                    int dashfound =0;
                    char* tempargs = args[0];
                    for(int i = 0; i < strlen(args[0]); i++){
                    	if(*tempargs == '/') dashfound =1;
                    	tempargs++;
                    }
                    if(dashfound == 1){//*args[0] == '/'
                        struct stat pathStat;
                        if(stat(args[0], &pathStat) == 0){
                        	int status;
                		////pid_t parentproc =getpid();
                			pid_t pid = fork();
                        	if(pid == -1){//error in forking
                		    	perror("Fork error");
                			}
                			else if(pid == 0){//this is the child process
                				if(execve(args[0], args, envp) == -1){
                					fprintf(stderr, "Cannot run executable\n");
                					//exit(3);
                				}
                        	}
                        	else{
                        		waitpid(-1, &status, 0);
                        	}
                        }

                    }
                    else{
                        //printf("PATH : %s\n", getenv("PATH"));
                        int numArray[2];
                        //printf("PWD: %s\n", getenv("PWD"));
                        char* filepath = getenv("PATH");
                        numCharactersinString(filepath, numArray);
                        int count = numArray[0];
                        int numargs = numArray[1];
                        char buf[count];
                        strcpy(buf, filepath);
                        //addToBuffer(buf, getenv("PATH"), count);
                        char* execdirectories[numargs] ;
                        char *token = strtok(buf, ":");
                        int i =0;
                        while(token != NULL){
                            execdirectories[i] = token;
                            token =strtok(NULL, ":");
                            i++;
                        }
                        for(i=0; i<numargs; i++){
                            //printf("Directories: %s\n", execdirectories[i]);
                        }
                        int found =0;
                        for(int i =0; i< numargs; i++){
                            //char* directory = malloc(strlen(execdirectories[i]));
                            char* temp  = execdirectories[i];
                            //printf("Directory(i = %d): %s\n", i,temp);
                            // int count =0;
                            // while(*temp != '\0'){
                            //     count++;
                            //     temp++;
                            // }
                            // temp = temp -count;
                            //char* directory = malloc(strlen(temp) + strlen(args[0]) + 1);
                            char directory[strlen(temp) + strlen(args[0]) + 1];
                            strcpy(directory, execdirectories[i]);
                            strcat(directory, "/");
                            strcat(directory, args[0]);
                            //printf("After directory print %s\n", directory);
                            struct stat pathStat;
                            if(stat(directory, &pathStat) == 0){
                                found = 1;
                                //printf("Found executable!\n");
                                int redirectIndexes[2];
                                redirectIndexes[0] = -1;
                                redirectIndexes[1] = -1;
                                int redirCount = 0;
                                int redirInfirstorLast = 0;
                                for(int i =0; i < (argcount -1); i++){//look through the args array for redirection symbols till the last one since the last is null
                                    if((strcmp(args[i], "<")== 0)|| (strcmp(args[i], ">")== 0)|| (strcmp(args[i], "|") == 0) ){
                                        if((i != 0) && (i != (argcount -2))){
                                            redirectIndexes[redirCount] = i;
                                            redirCount++;
                                        }
                                        else{
                                            redirInfirstorLast = 1;
                                            fprintf(stderr,"bash: syntax error near unexpected token `newline'\n");
                                            //exit(3);
                                        }
                                    }
                                }
                                int redirSupported = 0;
                                if((redirCount == 0) && (redirInfirstorLast == 0)){
                                    int status;
		                			pid_t pid = fork();
		                        	if(pid == -1){//error in forking
		                		    	fprintf(stderr,"Fork error");
		                			}
		                			else if(pid == 0){//this is the child process

		                            	if(execve(directory, args, envp)==-1){
		                            		fprintf(stderr, "Cannot run executable\n");
		                            		//exit(3);
		                            	}
		                        	}
		                        	else{
		                        		waitpid(-1, &status, 0);
		                        	}
                                    //execve(directory, args, envp);
                                    break;
                                }
                                if(redirCount > 0 && redirCount < 3){//REDIRECTION for 2 sybols only supported
                                    newMethod(redirCount, args, redirectIndexes, &redirSupported, argcount, input, output, directory, envp, execdirectories, numargs);
                                    //redirectionOrPiping(redirCount, args, redirectIndexes, redirSupported, argcount, input, output, directory, envp);
                                }
                                if(redirCount > 0 && redirSupported == 0){
                                    fprintf(stderr, "Redirection of this type is not supported\n");
                                }


                            }
                            //free(directory);
                            if(found == 1){
                                break;
                            }
                        }
                        if(found == 0){
                            fprintf(stderr,"bash: cd: %s: No such file or directory\n", args[0]);
                            //free(directory);
                        }
                        //char** directories = numexecDirectories(getenv("PATH"));
                    }

                //     exit(0);
                // }
                // else{//the parent process
                //     waitpid(-1, &status, 0);
                //     if(input != -1){
                //         close(input);
                //     }
                //     if(output != -1){
                //         close(output);
                //     }
                // }
            }

        }
        read = realloc(read, 15 + strlen(getenv("PWD")));
        char* temp = "<aanil> : <";
        strcpy(read, temp);
        strcat(read, getenv("PWD"));
        char* cat = "> $ ";
        strcat(read, cat);
        free(compare);
        free(formCmd);
        //printf("%s\n",cmd);
        /* All your debug print statements should use the macros found in debu.h */
        /* Use the `make debug` target in the makefile to run with these enabled. */
        //info("Length of command entered: %ld\n", strlen(cmd));

        /* You WILL lose points if your shell prints out garbage values. */
        }

    }



    /* Don't forget to free allocated memory, and close file descriptors. */
    free(cmd);
    cmd= NULL;
    free(read);

    return EXIT_SUCCESS;
}
