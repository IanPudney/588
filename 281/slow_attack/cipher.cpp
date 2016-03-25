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
#include <sys/select.h>
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

	int pipefd[2];
	auto size = strlen(str);
	auto piperet = pipe(pipefd);
	if (piperet < 0) {
		strcpy(str, strerror(errno));
		return;
	}

	auto fret = fcntl(pipefd[1], F_SETPIPE_SZ, MAX_PIPE_SIZE);
	if (fret < 0) {
		strcpy(str, strerror(errno));
		return;
	}


	/*fd_set rfds;
	struct timeval tv;
	FD_ZERO(&rfds);
	FD_SET(pipefd[1], &rfds);
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	select(pipefd[1] + 1, NULL, &rfds, NULL, &tv);*/

	auto stick_size = size + 1;
	if (stick_size > MAX_PIPE_SIZE) {
		stick_size = MAX_PIPE_SIZE;
	}

	auto writeret = write(pipefd[1], str, stick_size);
	if (writeret <= 0) {
		strcpy(str, strerror(errno));
		return;
	}

	str[0] = '\0';

	auto readret = read(pipefd[0], str, writeret);
	if (readret <= 0) {
		strcpy(str, strerror(errno));
		return;
	}

	breakCipherActual(str);

	close(pipefd[0]);
	close(pipefd[1]);
}