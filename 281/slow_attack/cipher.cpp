// Jordan Ridenour - Homework 1

#include <iostream>
#include <cstring>
using namespace std;

volatile int i = 36;

volatile int fibonacci(volatile int in) {
	int* alternate = new int(in);
	if (in == 0) {
		return 0;
	}
	if (in == 1) {
		return 1;
	}
	volatile int ret = fibonacci(*alternate - 1) + fibonacci(*alternate - 2);
	delete alternate;
	return ret;
}

void breakCipher(char *str) {
	//make the program use tons of time and memory
	volatile int x = fibonacci(i);
	i = 36;


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
