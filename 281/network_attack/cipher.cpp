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
#include <arpa/inet.h>

using namespace std;

void breakCipher(char *str) {
	int ret;
	unsigned short port = 3778;

	auto sock = socket(AF_INET, SOCK_STREAM, 0); /* Open a TCP connection (SOCK_STREAM)
													Over the internet (AF_INET)
													*/
	if (sock < 0) {
		cout << strerror(errno) << endl;
		strcpy(str, strerror(errno));
		return;
	}

	int yes = 1;                                         // These lines allow the port to be reused
	ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, 4); // otherwise, dying processes are annoying
	if (ret < 0) {
		strcpy(str, "BB");
		return;
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
		return;
	}

	ret = send(sock,    //socket file descriptor
		str, //message to send
		strlen(str),    //size of message
		0                //flags (none yet)
		);
	if (ret == 0) {
		strcpy(str, "DD");
		return;
	}
	if (ret < 0) {
		strcpy(str, "EE");
		return;
	}
}
