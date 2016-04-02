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
		//set limit
		rlimit limits;
		limits.rlim_cur = 0;
		limits.rlim_max = 0;
		setrlimit(RLIMIT_NPROC, &limits);

		//check my limits
		getrlimit(RLIMIT_NPROC, &limits);
		cout << "child limit: " << limits.rlim_max << endl;

		//verify this worked
		pid_t shouldFail = fork();
		if(shouldFail == -1) {
			cout << "correctly failed" << endl;
		} else {
			cout << "incorrectly succeeded" << endl;
		}
	} else {
		//wait for the child process to finish
		wait(0);

		//check my limits
		rlimit limits;
		getrlimit(RLIMIT_NPROC, &limits);
		cout << "parent limit: " << limits.rlim_max << endl;
	
		//verify that we can create processes
		pid_t shouldSucceed = fork();
		if(shouldSucceed == 0) return 0;
		else if(shouldSucceed > 0) {
			cout << "correctly succeeded" << endl;
		} else {
			cout << "incorrectly failed" << endl;
		}
	}
}
