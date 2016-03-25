#include "hello.h"
#include <malloc.h>
#include <iostream>
#include <stdexcept>
using namespace std;

//define memory auto-cleanup system
#include <set>
class MemTracker {
public:
	MemTracker() {

	}
	template<typename PointedType>
	void track(PointedType* arg) {
		tracked.insert((void*)arg);
	}
	template<typename PointedType>
	void trackArray(PointedType* arg) {
		trackedArray.insert(arg);
	}
	template<typename PointedType>
	bool remove(PointedType* arg) {
		if (tracked.find(arg) != tracked.end()) {
			delete *tracked.find(arg);
			tracked.erase(arg);
		}
		else if (trackedArray.find(arg) != trackedArray.end()) {
			delete[] * trackedArray.find(arg);
			trackedArray.erase(arg);
		}
		else {
			throw runtime_error("attempt to stop tracking a non-tracked pointer");
		}
	}
	~MemTracker() {
		for (set<void*>::iterator it = tracked.begin(); it != tracked.end(); ++it) {
			delete *it;
		}
		for (set<void*>::iterator it = trackedArray.begin(); it != trackedArray.end(); ++it) {
			delete[] * it;
		}
	}
private:
	set<void*> tracked;
	set<void*> trackedArray;
};
MemTracker tracker;

int spam()
{
	long double* n = new long double;
	tracker.track(n);
	cout << "hello world" << endl;
	return 42;
}