#include <string>
#include <iostream>
#include <string.h>
using namespace std;

void breakCipher(char *str);

void debug(string str) {
	char* strToTest = strdup(str.c_str());
	breakCipher(strToTest);
	cout << "encrypted: " << strToTest << endl;
	breakCipher(strToTest);
	cout << "decrypted: " << strToTest << endl;
	free(strToTest);
}

int main() {
	debug("Hi! My name is Jordan Ridenour.");
	
}
