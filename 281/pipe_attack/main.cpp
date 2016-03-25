#include <string>
#include <iostream>
#include <string.h>
using namespace std;

void breakCipher(char *str);

void debug(string str) {
	char* strToTest = strdup(str.c_str());
	breakCipher(strToTest);
	breakCipher(strToTest);
	if (str == strToTest) {
		cout << "yay!" << endl;
	}
	else {
		cout << strToTest << endl;
	}
	free(strToTest);
}

int main() {
	string x;
	for (int i = 0; i < 1024 * 1024 * 2; ++i) {
		x += (char)((i % 8) + 32);
	}
	debug(x);
	
}
