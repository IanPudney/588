#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;
int main() {
	pid_t pid = fork();
	if(pid == 0) {
		//sandbox this, so it doesn't affect other processes of the same user
		unshare(CLONE_NEWUSER);

		//set limit
		rlimit limits;
		limits.rlim_cur = 0;
		limits.rlim_max = 0;
		setrlimit(RLIMIT_NPROC, &limits);

		//verify this worked
		pid_t shouldFail = fork();
		if(shouldFail == -1) {
			cout << "correctly failed" << endl;
		} else {
			cout << "incorrectly succeeded" << endl;
		}
	} else {
		int status;
		waitpid(pid, &status, 0);
		//verify that other proceses were unaffected
		pid_t shouldSucceed = fork();
		if(shouldSucceed == 0) return 0;
		else if(shouldSucceed > 0) {
			cout << "correctly succeeded" << endl;
		} else {
			cout << "incorrectly failed" << endl;
		}
	}
}
