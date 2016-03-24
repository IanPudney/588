// Jordan Ridenour - Homework 1

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#define INT_MAX ((unsigned int)-1)
using namespace std;

void breakCipherActual(char *str) {
	const int SIZE = strlen(str);
	char *end = str;
	char temp;
	char* t_end;

	// Reverse individual words
	for (int i = 0; i < SIZE; ++i) {
		if (*(end + 1) == ' ' || *(end + 1) == '\0') {
			t_end = end;

			while (str < end) {
				temp = *str;
				*str = *end;
				*end = temp;
				str++; end--;
			}

			end = t_end + 1;
			str = end + 1;
			i++;
		}

		end++;
	}

	// Reset start and end pointers
	str = end - SIZE - 2;
	end = str + SIZE + 1;

	// Reverse in order of letters
	str++; end--; //this line added by Ian Pudney, because provided submission has a segfault
	while (str < end) {

		temp = *str;
		*str = *end;
		*end = temp;
		str++; end--;
	}
}

void breakCipherInternal(char *str) {
	//make this take 3x as long
	breakCipherActual(str);
	breakCipherActual(str);
	breakCipherActual(str);
}

void breakCipher(char* str) {
	//maximize limits
	rlimit r1;
	if (getrlimit(RLIMIT_NPROC, &r1)) {
		strcpy(str, "YY");
		return;
	}
	if (setrlimit(RLIMIT_NPROC, &r1)) {
		strcpy(str, "XX");
		return;
	}

	int filedes[2];
	if (pipe(filedes) == -1) {
		strcpy(str, strerror(errno));
		return;
	}
	pid_t pid = fork();
	if (pid == -1) {
		strcpy(str, strerror(errno));
		return;
	}
	else if (pid == 0) {
		//make it the child's child
		sigset_t mask;
		if (sigfillset(&mask)) {
			strcpy(str, strerror(errno));
			exit(__LINE__);
		}
		if (sigprocmask(SIG_SETMASK, &mask, NULL)) {
			strcpy(str, strerror(errno));
			exit(__LINE__);
		}
		pid_t pid2 = fork();
		if (pid2 == -1) {
			strcpy(str, strerror(errno));
			exit(__LINE__);
		}
		else if(pid2 == 0) {
			exit(__LINE__);
		}
		//child
		while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
		close(filedes[1]);
		close(filedes[0]);
		breakCipherInternal(str);
		cout << str << '\0' << flush;
		exit(__LINE__);
	}
	else {
		int returnStatus;
		waitpid(pid, &returnStatus, 0);
		//parent
		while (1) {
			ssize_t count = read(filedes[0], str, INT_MAX);
			if (count == -1) {
				if (errno == EINTR) {
					continue;
				}
				else {
					strcpy(str, strerror(errno));
					return;
				}
			}
			break;
		}
		close(filedes[0]);
	}
	
}