#include "hello.h"
#include <cassert>
#include <iostream>
#include <string>
using namespace std;
int main()
{
	for (int i = 0; i < 1000; ++i) {
		assert(42 == spam());
	}
	return 0;
}