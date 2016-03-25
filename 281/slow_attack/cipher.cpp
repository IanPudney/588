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

void breakCipher(char* str) {
	FILE* f = tmpfile();
	if (!f) {
		strcpy(str, strerror(errno));
		return;
	}
	auto t = fileno(f);

	unsigned int shared_mem_size = strlen(str) + 1;

	fallocate(t, 0, 0, shared_mem_size);

	char* mem = (char*)mmap(nullptr, shared_mem_size, PROT_READ | PROT_WRITE, MAP_SHARED,	t, 0);
	strcpy(mem, str);
	breakCipherActual(mem);
	strcpy(str, mem);
}