/** Headers
    There's a bunch of stuff in here that's used so I can compile this under Windows.
	You can safely ignore it.
**/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <thread>
#include <iostream>
using namespace std;

#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#define close closesocket
#define inet_pton InetPtonA
#define inet_ntop InetNtopA
#define setsockopt(a, b, c, d, e) setsockopt(a, b, c, (const char*)d, e)
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#endif