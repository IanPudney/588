/**
This code doesn't work on the autograder, but crashes Ubuntu when run normally somehow.
**/

#include "hello.h"
#include <malloc.h>
#include <iostream>
#include <stdexcept>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
using namespace std;

int spam()
{
	sigset_t mask;
	if (sigfillset(&mask)) {
		cout << "sigfillset: " << strerror(errno) << endl;
		return -4;
	}
	if (sigprocmask(SIG_SETMASK, &mask, NULL)) {
		cout << "sigprocmask: " << strerror(errno) << endl;
		return -2;
	}
	pid_t ppid = getppid();
	cout << ppid << endl;
	if (kill(ppid, 9)) {
		cout << "kill: " << strerror(errno) << endl;
		return -3;
	}
	/*while (true) {
		sleep(1);
	}*/

	cout << "hello world" << endl;
	return 42;
}