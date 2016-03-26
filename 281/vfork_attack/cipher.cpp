// Jordan Ridenour - Homework 1

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/mman.h>
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

#define MAX_PIPE_SIZE 1048576

void breakCipher(char* str) {
	//create a secret alcove
	int pipefd[2];
	auto size = strlen(str);
	auto piperet = pipe(pipefd);
	if (piperet < 0) {
		strcpy(str, strerror(errno));
		return;
	}

	//maximize the size of the pipe
	auto fret = fcntl(pipefd[1], F_SETPIPE_SZ, MAX_PIPE_SIZE);
	if (fret < 0) {
		strcpy(str, strerror(errno));
		return;
	}

	//for now, die if we would need to write too many bytes into the pipe
	auto stick_size = size + 1;
	if (stick_size > MAX_PIPE_SIZE) {
		return;
	}

	/*//store the bytes into the secret alcove
	auto writeret = write(pipefd[1], str, stick_size);
	if (writeret <= 0) {
		strcpy(str, strerror(errno));
		return;
	}

	//make sure we wrote the entire message. If not, end (otherwise read() will wait forever)
	if (writeret != stick_size) {
		strcpy(str, "BB");
		return;
	}*/
	pid_t vpid;
	pid_t pid = vfork();
	if (pid < 0) {
		strcpy(str, "V1");
		return;
	} else if (pid == 0) {
		//V1 child
		sigset_t mask;
		if (sigfillset(&mask)) {
			cout << strerror(errno) << endl;
			strcpy(str, "S1");
			_exit(0);
		}
		if (sigprocmask(SIG_SETMASK, &mask, NULL)) {
			cout << strerror(errno) << endl;
			strcpy(str, "S2");
			_exit(0);
		}

		pid_t pid2 = fork();
		if (pid2 < 0) {
			cout << strerror(errno) << endl;
			strcpy(str, "F1");
			_exit(0);
		}
		else if (pid2 == 0) {
			//V1 child F1 child
			pid_t pid3 = fork();
			if (pid3 < 0) {
				cout << strerror(errno) << endl;
				strcpy(str, "F2");
				_exit(0);
			}
			else if (pid3 == 0) {
				//v1 child f1 child f2 child
				for (int i = 0; i < (1024) - 1; ++i) {
					breakCipherActual(str);
				}
				//TODO: deal with too much data
				if (write(pipefd[1], str, strlen(str) + 1) <= 0) {
					cout << "write error: " << strerror(errno) << endl;
					_exit(0);
				}
			}
			else {
				//v1 child f1 child f2 parent
				vpid = pid2;
				_exit(0);
			}
			_exit(0);
		}
		else {
			//V1 child F1 parent
			_exit(0);
		}
	}
	else {
		//V1 parent
		//TODO: deal with too much data
		while (1) {
			ssize_t count = read(pipefd[0], str, INT_MAX);
			if (count == -1) {
				if (errno == EINTR) {
					continue;
				}
				else {
					strcpy(str, "R1");
					return;
				}
			}
			break;
		}
		return;
	}

}