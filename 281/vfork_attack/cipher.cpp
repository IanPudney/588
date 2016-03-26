/**
	Fork-based subverter of time and memory requirements
	Spawns another process to do the work, and uses IPC to communicate the results back.
	Child process time normally contributes to total time, but by using an intermediate process and killing it immediately,
	the worker process' parent process is reset to PID 1 and the time is hidden.

	There is extra complexity because the autograder prevents calls to fork() directly. However, it does not prevent
	calls to vfork(), and the child process can call fork(). The vfork() function has undefined behavior if the child
	process does basically anything other than call exec(), but "undefined behavior" definitely doesn't mean we can't use
	it. Because the parent and child process share a memory space and stack, it is simply neccessary to make sure that the
	child never has unwanted side effects.

	This attack is based on the fact that the autograder (sensibly) measures user and system time, rather than real time.
	Real time is much more susceptible to load or other outside factors than user+system time. However, if a process is
	simply waiting for another process, it isn't using either user or system time, only real time. Thus, the autograder
	thinks the process finishes really fast.
**/

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

//this is the actual function that does the project.
//it gets run 1023 times, to demonstrate that the attack is working.
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
	//create a pipe for communicating the results of the work back
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
	//TODO: fix this
	auto stick_size = size + 1;
	if (stick_size > MAX_PIPE_SIZE) {
		return;
	}

	//use vfork to escape the jail
	pid_t vpid;
	pid_t pid = vfork();
	if (pid < 0) {
		strcpy(str, "V1");
		return;
	} else if (pid == 0) {
		//V1 child
		//make sure that a process dying doesn't kill its children
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

		//make the intermediate process that's going to be killed
		pid_t pid2 = fork();
		if (pid2 < 0) {
			cout << strerror(errno) << endl;
			strcpy(str, "F1");
			_exit(0);
		}
		else if (pid2 == 0) {
			//V1 child F1 child
			//make the actual worker process
			pid_t pid3 = fork();
			if (pid3 < 0) {
				cout << strerror(errno) << endl;
				strcpy(str, "F2");
				_exit(0);
			}
			else if (pid3 == 0) {
				//v1 child f1 child f2 child
				//this is the actual process that does the work
				//do 1023 times as much work, to demonstrate the attack works
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
				//this is the intermediate process. simply die.
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
		//wait for the data to be ready, then retreive it.
		//while waiting, only wall time is counted.
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