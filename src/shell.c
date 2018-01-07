#include "shell.h"
#include <fcntl.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>

char* checkCMD(char* cd){
	char* temp = malloc(strlen(cd));
	strcpy(temp, cd);
	char tempCMD[256];
	int count =0;
	char *addr = temp;
	char* formCmd = strsep(&temp, " ");
	int numargs =0;
	while(formCmd != NULL){
		if(strcmp(formCmd, "") !=0){
			numargs++;
		}
		formCmd = strsep(&temp, " ");
	}
	// if(numargs == 0 || numargs > 2){
	// 	return NULL;
	// }
	if(numargs == 0){
	 	return NULL;
	}
	//char* tempTmp = NULL;
	if(numargs > 2){
		free(addr);
		temp= malloc(strlen(cd));
		addr = temp;
		strcpy(temp,cd);//add while
		char *tempTmp = addr;
		for(int i =0; i< strlen(cd); i++){
			*tempTmp = '*';
			tempTmp++;
		}
		return temp;
	}
	free(addr);
	temp= malloc(strlen(cd));
	strcpy(temp,cd);//add while
	addr = temp;
	formCmd = strsep(&temp, " ");
	while(strcmp(formCmd, "")==0){
		formCmd = strsep(&temp, " ");
	}
	int argcount =0;
	while(argcount != numargs){
		//strcpy(tempCMD, formCmd);
		if(strcmp(formCmd, "")==0){
			formCmd = strsep(&temp, " ");
			while(strcmp(formCmd, "")==0){
				formCmd = strsep(&temp, " ");
			}
			if(argcount != numargs){
				tempCMD[count] = ' ';
				count++;
			}
		}
		char* tempForm = formCmd;
		while(*tempForm != '\0'){
			tempCMD[count] = *tempForm;
			count++;
			tempForm++;
		}
		argcount++;
		formCmd = strsep(&temp, " ");
		if(formCmd!= NULL){
			if(strcmp(formCmd, "") !=0){
				tempCMD[count] = ' ';
				count++;
			}
		}
	}
	free(addr);
	tempCMD[count] = '\0';
	char* returned = malloc(count);
	addr = returned;
	int i =0;
	char* tempret = returned;
	while(i <= count){
		*tempret = tempCMD[i];
		i++;
		tempret++;
	}
	//tempret = tempret - count;
	return returned;
}

void numCharactersinString(char* filepath, int numArray[2]){
    //int numArray[2];
    int count =0;
    while(*filepath != '\0'){
        count++;
        filepath++;
    }
    filepath = filepath - count;
    char buf[count];
    int numargs =0;
    for(int i =0; i< count; i++){
        buf[i] = *filepath;
        filepath++;
        if(buf[i] == ':'){
            numargs++;
        }
    }
    numargs++;
    numArray[0] = count;
    numArray[1] = numargs;
    //return count;
}



//TODO: MULTIPLE FD OPEN SINCE MULTIPLE FILES ARE OPEN
void newMethod(int redirCount, char** args, int* redirectIndexes, int* redirSupported, int argcount, int input, int output, char* directory, char** envp, char** execdirectories, int numargs){
	if(redirCount == 1){
		*redirSupported =1;
		char* inputargs[redirectIndexes[0] +1];
        int i =0;
        for(i = 0; i< redirectIndexes[0];i++){
        	inputargs[i] = args[i];
        }
        inputargs[i] = (char*) 0;
        int j =0;
	    //char* outputargs[argcount-redirectIndexes[0] -2];
	    for(int i = redirectIndexes[0]+1; i< argcount -1;i++){
	    	//outputargs[j] = args[i];
	    	j++;
	    }
        if(j == 0){
        	fprintf(stderr, "bash: syntax error near unexpected token `newline'\n");
        	//exit(3);
        }
    	else{
    		if((strcmp(args[redirectIndexes[0]], "<") == 0) || (strcmp(args[redirectIndexes[0]], ">") == 0)){
	            if(strcmp(args[redirectIndexes[0]], "<") == 0){
	                    //reads the file after the redirection if no input given for <
	            		output = open(args[argcount-2], O_RDONLY);
	            		//you dont have to check if the output is valid since the exec would do it
	        			int status;
	        			int pid = fork();
	        			if(pid == 0){
	            			dup2(output, STDIN_FILENO);
	            			close(output);
	            			//printf("Run %s\n", directory);
	            			if(execve(directory, inputargs, envp) == -1){
	        					fprintf(stderr, "Cannot run executable\n");
	        					//exit(3);
	        				}
	        				// if(execle(directory, args[0], args[argcount -2], (char*) 0, envp) == -1){
	        				// 	fprintf(stderr, "Cannot run executable\n");
	        				// 	exit(3);
	        				// }
	        				exit(0);
	        			}
	        			else{
	        			 close(output);
	        				waitpid(-1, &status, 0);
	        			}
	            }
	            if(strcmp(args[redirectIndexes[0]], ">") == 0){
	            	*redirSupported = 1;
		    		output = open(args[redirectIndexes[0] +1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR |S_IRGRP|S_IWUSR);
	    			int status;
	    			int pid = fork();
	    			if(pid == 0){
	    				dup2(output, 1);
	    				close(output);
	    				//printf("Run %s\n", directory);
	    				if(execve(directory, inputargs, envp) == -1){
	    					fprintf(stderr, "Cannot run executable");
	    					//exit(3);
	    				}
	    				exit(0);

	    			}
	    			else{
	    				close(output);
	    				waitpid(-1, &status, 0);
	    			}
				}
	    	}
	        if(strcmp(args[redirectIndexes[0]], "|") == 0){
	            //TODO
	            *redirSupported = 1;
	            //printf("|\n");
	            char* inputargs2[argcount - redirectIndexes[0] -1];
		        int i =0;
		        int j =0;
		        for(i = redirectIndexes[0] +1; i< argcount -1;i++){
		        	inputargs2[j] = args[i];
		        	j++;
		        }
		        inputargs2[j] = (char*) 0;
		        int foundarg2 = 0;
		        int numArray[2];
		        char* filepath = getenv("PATH");
	            numCharactersinString(filepath, numArray);
	            int count = numArray[0];
	            int numargs = numArray[1];
	            char buf[count];
	            strcpy(buf, filepath);
	            //addToBuffer(buf, getenv("PATH"), count);
	            char* execdirectories[numargs] ;
	            char *token = strtok(buf, ":");
	            int a =0;
	            while(token != NULL){
	                execdirectories[a] = token;
	                token =strtok(NULL, ":");
	                a++;
	            }
		        char tempInput[strlen(inputargs2[0])];
		        char* pipedir;
		        int strlength = 0;
	        	for(int i = 0; i < numargs; i++){
	        		if(foundarg2 != 1){
			    		strcpy(tempInput, inputargs2[0]);
			    		char* temp = execdirectories[i];
			    		char directory2[strlen(temp) + strlen(tempInput) + 1];
			    		strcpy(directory2, temp);
			            strcat(directory2, "/");
			            strcat(directory2, tempInput);
			            //printf("After directory print %s\n", directory);
			            struct stat pathStat;
			            if(stat(directory2, &pathStat) == 0){
			            	foundarg2 = 1;
			            	pipedir = directory2;
			            	strlength = strlen(pipedir);
			            	//printf("pipedirectory1: %s\n", pipedir);
			            	pipedir = malloc(strlength);
			            	strcpy(pipedir, directory2);
			            }
	            	}
	        	}
	        	if(foundarg2 == 0){
	        		fprintf(stderr, "No such file exists: %s\n", inputargs2[0]);
	        		//exit(3);
	        	}
	        	else{
	        		int fd[2];
		            if(pipe(fd) == -1){
		            	fprintf(stderr, "Error in pipe()\n" );
		            	exit(3);
		            }
		            else{
		            	int status;
			            //int status2;
			            //printf("Finished piping\n");
			            //printf("pipedirectory: %s\n", pipedir);
			            //pid_t pid2;

			            pid_t pid = fork();
			            //fortune | cowsay
			            if(pid == -1){
			            	fprintf(stderr,"Fork error\n");
			            }
			            if (pid == 0)
					    {
					    	//close(fd[1]);
						    	//printf("RUn fortune\n");
						    	//printf("First program: %s\n", directory);
						    	close(fd[0]);
						      	dup2(fd[1], STDOUT_FILENO);

						      	close(fd[1]);
						      // close unused unput half of pipe
						      // execute FORTUNE
						      	if(execve(directory, inputargs, envp) == -1){
									fprintf(stderr, "Cannot run executable2\n");
									//fprintf(stderr, "%d\n", errno);
									//exit(3);
								}

								exit(0);
					    }
					  else
					    {
					      // replace standard output with output part of pipe
					    	//waitpid(pid, &status, 0);
					    	pid_t pid2;
					    	int status2;
					    	if((pid2 =fork())==0){
								//printf("RUn cowsay\n");
					      		// replace standard input with input part of pipe
					      		close(fd[1]);
					      		dup2(fd[0], STDIN_FILENO);
					      		close(fd[0]);
					      		// close unused hald of pipe

					      		// printf("Inputargs2[0]: %s\n", inputargs2[0]);
					      		// printf("Inputargs2[1]: %s\n", inputargs2[1]);
					      		// printf("pipedirectory: %s\n", pipedir);
					      		if(execve(pipedir, inputargs2, envp) == -1){
									fprintf(stderr, "Cannot run executable1\n");
									//fprintf(stderr, "%d\n", errno);
								//exit(3);
								}
								//waitpid(pid, &status, 0);

								exit(0);
							}
							else{
								close(fd[0]);
								close(fd[1]);
								//printf("pid1 = %i\n pid2 = %i\n", pid, pid2);
								//printf("Waiting for dead children...");
								waitpid(pid, &status, 0);
								waitpid(pid2, &status2, 0);

								//printf("Kk, children ded.");

							}
					    }
		            }
	        	}
	        }
    	}
    }
    else{//reidrection is 2
        if((strcmp(args[redirectIndexes[0]], "<") == 0) && (strcmp(args[redirectIndexes[1]], ">") == 0)){
            *redirSupported = 1;
            //TODO
            //printf("< >\n");
            if(redirectIndexes[0] == (redirectIndexes[1]-1)){
            	fprintf(stderr, "bash: syntax error near unexpected token `>'\n");
            	//exit(3);
            }
            if(redirectIndexes[1] == (argcount-2)){//no arguments after > : Error
            	fprintf(stderr, "bash: syntax error near unexpected token `newline'\n");
            	//exit(3);
            }
            else{
            	int i = 0;
	        	char* inputargs[redirectIndexes[0] +1];
	        	while(i != redirectIndexes[0]){
	        		inputargs[i] = args[i];
	        		//printf("%s\n", inputargs[i]);
	        		i++;
	        	}
	        	inputargs[i] = (char*) 0;
	        	input = open(args[redirectIndexes[0] +1], O_RDONLY);
	        	if(input == -1){
	        		fprintf(stderr, "Input file %s does not exist\n", args[redirectIndexes[0] +1]);
	        		//exit(3);
	        	}
	        	else{
	        		output = open(args[redirectIndexes[1] +1], O_WRONLY | O_CREAT| O_TRUNC, S_IRUSR |S_IRGRP|S_IWUSR);
	            	int status;
	            	//printf("redirSupported: %d\n", redirSupported);
	    			int pid = fork();
	    			if(pid == 0){
	    				dup2(output, STDOUT_FILENO);
	    				close(output);
	    				dup2(input, STDIN_FILENO);
	    				close(input);
	    				if(execve(directory, inputargs, envp) == -1){
	    					fprintf(stderr, "Cannot run executable");
	    					exit(3);
	    				}
	    			}
	    			else{
	    				close(output);
	    				close(input);
	    				waitpid(-1, &status, 0);
	    			}
	        	}
            }

        }

        if((strcmp(args[redirectIndexes[0]], "|") == 0) && (strcmp(args[redirectIndexes[1]], "|") == 0)){
            //TODO
            //printf("| |\n");
            *redirSupported = 1;
            char* inputargs[redirectIndexes[0] +1];
        	int i =0;
        	for(i = 0; i< redirectIndexes[0];i++){
        	inputargs[i] = args[i];
        	}
        	inputargs[i] = (char*) 0;
        	int argsafterpipe1 =0;
	    //char* outputargs[argcount-redirectIndexes[0] -2];
	    	for(int i = redirectIndexes[0]+1; i< redirectIndexes[1]; i++){
	    	//outputargs[j] = args[i];
	    		argsafterpipe1++;
	    	}
            char* inputargs2[redirectIndexes[1] - redirectIndexes[0]];
	        i =0;
	        int j =0;
	        for(i = redirectIndexes[0] +1; i< redirectIndexes[1];i++){
	        	inputargs2[j] = args[i];
	        	j++;
	        }
	        inputargs2[j] = (char*) 0;
	        int argsafterpipe2 =0;
	    //char* outputargs[argcount-redirectIndexes[0] -2];
	    	for(int i = redirectIndexes[1]+1; i< argcount -1;i++){
	    	//outputargs[j] = args[i];
	    		argsafterpipe2++;
	    	}
            char* inputargs3[argcount - redirectIndexes[1] -1];
	        i =0;
	        j =0;
	        for(i = redirectIndexes[1] +1; i< argcount -1;i++){
	        	inputargs3[j] = args[i];
	        	j++;
	        }
	        inputargs3[j] = (char*) 0;
	        if((argsafterpipe1 == 0) || (argsafterpipe2 == 0)){
	        	fprintf(stderr, "Not enough arguments to pipe\n");
	        }
	        else{
	        	int foundarg2 = 0;
		        int numArray[2];
		        char* filepath = getenv("PATH");
	            numCharactersinString(filepath, numArray);
	            int count = numArray[0];
	            int numargs = numArray[1];
	            char buf[count];
	            strcpy(buf, filepath);
	            //addToBuffer(buf, getenv("PATH"), count);
	            char* execdirectories[numargs] ;
	            char *token = strtok(buf, ":");
	            int a =0;
	            while(token != NULL){
	                execdirectories[a] = token;
	                token =strtok(NULL, ":");
	                a++;
	            }
	            char* filepath2 = getenv("PATH");
	            numCharactersinString(filepath, numArray);
	            count = numArray[0];
	            numargs = numArray[1];
	            char buf2[count];
	            strcpy(buf2, filepath2);
	            char* execdirectories2[numargs];
	            char* token2 = strtok(buf2, ":");
	            a =0;
	            while(token2 != NULL){
	                execdirectories2[a] = token2;
	                token2 =strtok(NULL, ":");
	                a++;
	            }
		        char tempInput[strlen(inputargs2[0])];
		        char* pipedir;
		        char* pipedir2;
		        int foundarg3 =0;
		        int strlength = 0;
		        char tempInput2[strlen(inputargs3[0])];
	        	for(int i = 0; i < numargs; i++){
	        		if(foundarg2 != 1){
			    		strcpy(tempInput, inputargs2[0]);
			    		char* temp = execdirectories[i];
			    		char directory2[strlen(temp) + strlen(tempInput) + 1];
			    		strcpy(directory2, temp);
			            strcat(directory2, "/");
			            strcat(directory2, tempInput);
			            //printf("After directory print %s\n", directory);
			            struct stat pathStat;
			            if(stat(directory2, &pathStat) == 0){
			            	foundarg2 = 1;
			            	pipedir = directory2;
			            	strlength = strlen(pipedir);
			            	//printf("pipedirectory1: %s\n", pipedir);
			            	pipedir = malloc(strlength);
			            	strcpy(pipedir, directory2);
			            }
	            	}
	        	}
	        	for(int i = 0; i < numargs; i++){
	        		if(foundarg3 != 1){
			    		strcpy(tempInput2, inputargs3[0]);
			    		char* temp = execdirectories2[i];
			    		char directory2[strlen(temp) + strlen(tempInput2) + 1];
			    		strcpy(directory2, temp);
			            strcat(directory2, "/");
			            strcat(directory2, tempInput2);
			            //printf("After directory print %s\n", directory);
			            struct stat pathStat;
			            if(stat(directory2, &pathStat) == 0){
			            	foundarg3 = 1;
			            	pipedir2 = directory2;
			            	strlength = strlen(pipedir2);
			            	//printf("pipedirectory1: %s\n", pipedir);
			            	pipedir2 = malloc(strlength);
			            	strcpy(pipedir2, directory2);
			            }
	            	}
	        	}
	        	if((foundarg2 == 0) || (foundarg3 ==0)){
	        		if(foundarg2==0)
	        			fprintf(stderr, "No such executable exists: %s\n", inputargs2[0]);
	        		else
	        			fprintf(stderr, "No such executable exists: %s\n", inputargs3[0]);
	        		//exit(3);
	        	}
	        	else{
	        		int fd[2];
	        		int fd2[2];
		            if(pipe(fd) == -1){
		            	fprintf(stderr, "Error in pipe()\n" );
		            	//exit(3);
		            }
		            if(pipe(fd2) == -1){
		            	fprintf(stderr, "Error in pipe()\n" );
		            	//exit(3);
		            }
		            else{
		            	int status;
			            //int status2;
			            //printf("Finished piping\n");
			            //printf("pipedirectory: %s\n", pipedir);
			            //pid_t pid2;
			            pid_t pid2;
			            pid_t pid3;
			            pid_t pid = fork();
			            //fortune | cowsay
			            if(pid == -1){
			            	fprintf(stderr, "Fork error\n");
			            }
			            if (pid == 0)
					    {
					    	//close(fd[1]);
						    	//printf("RUn fortune\n");
						    	//printf("First program: %s\n", directory);
						    	close(fd[0]);
						    	close(fd2[0]);
						    	close(fd2[1]);
						      	dup2(fd[1], STDOUT_FILENO);

						      	close(fd[1]);
						      // execute FORTUNE
						      	if(execve(directory, inputargs, envp) == -1){
									fprintf(stderr, "Cannot run executable2\n");
									//fprintf(stderr, "%d\n", errno);
									//exit(3);
								}

								exit(0);
					    }
					  	else
					    {
					    	int status2;
					    	if((pid2 =fork())==0){
								//printf("RUn cowsay\n");
					      		// replace standard input with input part of pipe
					      		close(fd[1]);
					      		close(fd2[0]);
					      		dup2(fd[0], STDIN_FILENO);
					      		dup2(fd2[1], STDOUT_FILENO);
					      		close(fd[0]);
					      		close(fd2[0]);
					      		// close unused hald of pipe

					      		// printf("Inputargs2[0]: %s\n", inputargs2[0]);
					      		// printf("Inputargs2[1]: %s\n", inputargs2[1]);
					      		// printf("pipedirectory: %s\n", pipedir);
					      		if(execve(pipedir, inputargs2, envp) == -1){
									fprintf(stderr, "Cannot run executable1\n");
									//fprintf(stderr, "%d\n", errno);
								//exit(3);
								}
								//waitpid(pid, &status, 0);

								exit(0);
							}
							else{
								int status3;
								if((pid3 =fork())==0){
									//printf("RUn cowsay\n");
						      		// replace standard input with input part of pipe
						      		close(fd2[1]);
						      		close(fd[0]);
						      		close(fd[1]);
						      		dup2(fd2[0], STDIN_FILENO);
						      		close(fd2[0]);
						      		// close unused hald of pipe

						      		// printf("Inputargs2[0]: %s\n", inputargs2[0]);
						      		// printf("Inputargs2[1]: %s\n", inputargs2[1]);
						      		// printf("pipedirectory: %s\n", pipedir);
						      		if(execve(pipedir2, inputargs3, envp) == -1){
										fprintf(stderr, "Cannot run executable1\n");
										//fprintf(stderr, "%d\n", errno);
									//exit(3);
									}
									//waitpid(pid, &status, 0);

									exit(0);
								}
								else{
									close(fd[0]);
									close(fd[1]);
									close(fd2[0]);
									close(fd2[1]);
									//printf("pid1 = %i\n pid2 = %i\n", pid, pid2);
									//printf("Waiting for dead children...");
									waitpid(pid, &status, 0);
									waitpid(pid2, &status2, 0);
									waitpid(pid3, &status3, 0);
									//printf("Kk, children ded.");

								}

							}
					    }
		            }
        		}
	        }

        }


    }
}
int Atoui(char *inputWord){
    int sum =0;
    for(int i =0; i< (int) strlen(inputWord); i++){
    	if(sum != -1){
	        if((*inputWord >= '0' && *inputWord <= '9')){
	            sum = sum* 10;
	            sum = sum + (*(inputWord) - '0');
	            inputWord++;
	        }
	        else{
	            sum = -1;
	            break;
	        }
    	}
    }
    return sum;
}

char* inttocharacter(int num, char numberstring[]){
	int rem, count, x;
	count =0;
	x = num;
	while(x != 0){
		count++;
		x/=10;
	}
	for(int i =0; i < count; i++){
		rem= num %10;
		num/=10;
		int n = i+1;
		numberstring[count -n] = rem + '0';
	}

	numberstring[count] = '\0';
	return numberstring;
}

int stringlength(char* string){
	return (strlen(string));
}
