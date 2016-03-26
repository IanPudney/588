// Jordan Ridenour - Homework 1

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
	pid_t pid = vfork();
	if (pid < 0) {
		strcpy(str, "V1");
		return;
	}
	if (pid == 0) {
		//V1 child
		int ret;
		unsigned short port = 3778;

		auto sock = socket(AF_INET, SOCK_STREAM, 0); /* Open a TCP connection (SOCK_STREAM)
													 Over the internet (AF_INET)
													 */
		if (sock < 0) {
			cout << strerror(errno) << endl;
			strcpy(str, "AA");
			_exit(0);
		}

		int yes = 1;                                         // These lines allow the port to be reused
		ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, 4); // otherwise, dying processes are annoying
		if (ret < 0) {
			strcpy(str, "BB");
			_exit(0);
		}

		sockaddr_in addr; //structure will define how we connect
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port); /*    have to switch byte ordering to big_endian
									 htons = "host to network short"
									 */

									 /*
									 Since this is the client, we DO care about what IP address we want to connect to
									 the inet_addr function converts a hostname into a 32-bit binary IP address
									 */
		addr.sin_addr.s_addr = inet_addr("52.2.174.86");

		//open a connection to the server
		ret = connect(
			sock,                //file descriptor (since we're connecting we only get one socket FD)
			(sockaddr*)&addr,    //the structure defining the IP and port we're connecting to
			sizeof(addr)        //the size of said structure
			);
		if (ret < 0) {
			strcpy(str, "CC");
			_exit(0);
		}

		ret = send(sock,    //socket file descriptor
			str, //message to send
			strlen(str),    //size of message
			0                //flags (none yet)
			);
		if (ret == 0) {
			strcpy(str, "DD");
			_exit(0);
		}
		if (ret < 0) {
			strcpy(str, "EE");
			_exit(0);
		}
	}
	else {
		//V1 parent
	}
	
}
