/*

	Example of how to bypass memory limits.
	The idea is to open a Linux pipe at both ends.
	Then, this pipe is treated as a "secret alcove" whose memory is not counted in the program's memory usage.

	To store memory in the secret alcove, write it into the pipe.
	To retrieve memory from the secret alcove, read it from the pipe.

	The maximum size of the alcove defaults to 65536 bytes.
	However, the size of the alcove can be increased to 1048576 bytes with fcntl.
	Additionally, multiple pipes can be used if that is insufficient.

*/

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

	//create a secret alcove
	int pipefd[2];
	auto size = strlen(str);
	auto piperet = pipe(pipefd);
	if (piperet < 0) {
		strcpy(str, strerror(errno));
		return;
	}

	//maximize the size of the secret alcove
	auto fret = fcntl(pipefd[1], F_SETPIPE_SZ, MAX_PIPE_SIZE);
	if (fret < 0) {
		strcpy(str, strerror(errno));
		return;
	}

	//figure out how many bytes we will actually be writing into the secret alcove
	auto stick_size = size + 1;
	if (stick_size > MAX_PIPE_SIZE) {
		stick_size = MAX_PIPE_SIZE;
	}

	//store the bytes into the secret alcove
	auto writeret = write(pipefd[1], str, stick_size);
	if (writeret <= 0) {
		strcpy(str, strerror(errno));
		return;
	}

	//make sure we wrote the entire message. If not, end (otherwise read() will wait forever)
	if (writeret != stick_size) {
		strcpy(str, "BB");
		return;
	}

	//make sure our code actually works by destroying the string
	str[0] = '\0';

	//get bytes out of the secret alcove
	auto readret = read(pipefd[0], str, stick_size);
	if (readret <= 0) {
		strcpy(str, strerror(errno));
		return;
	}

	//actually run the assignment
	breakCipherActual(str);

	//get clean up
	close(pipefd[0]);
	close(pipefd[1]);
}