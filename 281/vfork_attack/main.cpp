#include <string>
#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <iostream>
using namespace std;

void breakCipher(char *str);

void debug(string str) {
	char* strToTest = strdup(str.c_str());
	breakCipher(strToTest);
	cout << strToTest << endl;
	breakCipher(strToTest);
	cout << strToTest << endl;
	if (str == strToTest) {
		//cout << "yay!" << endl;
	}
	else {
		cout << strToTest << endl;
	}
	free(strToTest);
}

int main() {
	string x;
	x = "foo bar baz biz metasyntactic!";
	/*for (int i = 0; i < 1024 * 1024 * 2; ++i) {
		x += (char)((i % 8) + 32);
	}*/
	debug(x);
	
}
