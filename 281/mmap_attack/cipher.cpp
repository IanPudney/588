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

char tmpfilename[20];
void breakCipher(char* str) {
	strcpy(tmpfilename, "abcXXXXXX");
	auto t = mkstemp(tmpfilename);
	if (t < 0) {
		strcpy(str, "EE");
		return;
	}


	unsigned int shared_mem_size = strlen(str) + 1;
	

	/*auto fallret = fallocate(t, 0, 0, shared_mem_size);
	if (fallret < 0) {
		cout << strerror(errno) << endl;
		strcpy(str, "DD");
		return;
	}*/

	int result = lseek(t, shared_mem_size - 1, SEEK_SET);
	if (result == -1) {
		strcpy(str, "FF");
		return;
	}
	result = write(t, "", 1);
	if (result != 1) {
		strcpy(str, "GG");
		return;
	}

	char* mem = (char*)mmap(nullptr, shared_mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, t, 0);
	if (mem == 0) {
		strcpy(str, "BB");
		return;
	}
	if (mem == (char*)-1) {
		strcpy(str, "CC");
		return;
	}
	strcpy(mem, str);
	breakCipherActual(mem);
	strcpy(str, mem);
}