#include <filesystem>
#include "PCB.h"
#include "Memory.h"
#include <iostream>
#include <queue>
#include <vector>

Memory memory;
PCB* beingSwapped;
vector<PCB*> LTS;
vector<PCB*> IO;
queue<PCB*> swapOutQ;
bool swappingIn = false;
bool swappingOut = false;
bool doingIO = false;
void siodisk(int jobnum);
void siodrum(int jobnum, int jobsize, int coreaddress, int direction);
void ontrace(); 
void offtrace();
void runFromLTS(int &, int []);
void runCurrentJob(int &, int []);
void runIO(int &, int[]);
bool swapIn(int &, int[], PCB *, int);
void swapOut(int &, int[]);
void swapFromLTS(int &, int[]);


void startup()
{
	//ontrace();
}

// INTERRUPT HANDLERS
// The following 5 functions are the interrupt handlers. The arguments 
// passed from the environment are detailed with each function below.
// See RUNNING A JOB, below, for additional information
void Crint(int &a, int p[])
{
	cout << "new job #" << p[1]<< endl;
	// Indicates the arrival of a new job on the drum.
	// At call: p [1] = job number
	// p [2] = priority
	// p [3] = job size, K bytes
	// p [4] = max CPU time allowed for job
	// p [5] = current time
	PCB* temp = new PCB(p[1], p[2], p[3], p[4], -1);
	int memoryPos = memory.findMemPos(temp);
	if (memoryPos != -1 && swapIn(a, p, temp, memoryPos)) {
		cout << "swapped in new job #" << p[1] << endl;
	}
	else {
		LTS.push_back(temp);
		std::sort(LTS.begin(), LTS.end(), memory.sortByRemainingTime);
	}
	runCurrentJob(a, p);
}
void Dskint(int &a, int p[])
{
	cout << "disk interrupt" << " " << a << endl;
	doingIO = false;
	if (memory.getJobDoingIO()->shouldKill()) {
		cout << "killed it" << endl;
		memory.deleteFromMemory(memory.getJobDoingIO());
	}
	else if (memory.getJobDoingIO()->isBlocked()) {
		memory.getJobDoingIO()->setBlocked(false);
		memory.getJobDoingIO()->setDoingIO(false);
		memory.push(memory.getJobDoingIO());
	}
	else
		memory.getJobDoingIO()->setDoingIO(false);
	runIO(a, p);
	runCurrentJob(a, p);
}
void Drmint(int &a, int p[])
{
	cout << "drum interrupt" << " " << a << endl;
	if (swappingIn) {
		memory.insertNewJob(beingSwapped);
		swappingIn = false;
	}
	else {
		swappingOut = false;
	}
	runIO(a, p);
	runCurrentJob(a, p);
	swapFromLTS(a, p);
	swapOut(a, p);
}
void Tro(int &a, int p[])
{
	// Timer-Run-Out.
	// At call: p [5] = current time
	memory.getNextJob()->addCPUTime(p[5] - memory.getNextJob()->getPrevClock());
	if (memory.getNextJob()->getCPUTime() >= memory.getNextJob()->getMaxCPUTime()) {
		cout << "ran out of time" << endl;
		if (memory.getNextJob()->isDoingIO() || memory.getNextJob()->getPendingIO() > 0) {
			cout << "Will kill it" << endl;
			memory.killAfterIO(memory.getNextJob());
		}
		else
		{
			memory.deleteFromMemory(memory.getNextJob());
		}
	}
	runCurrentJob(a,p);
}
void Svc(int &a, int p[])
{
	cout << "svc request" << " " << a << endl;
	if (a == 6) {		//needs IO
		memory.getNextJob()->incrementPendingIO();
		IO.push_back(memory.getNextJob());
		runIO(a, p);
	}
	else if (a == 5) {		//terminated
		if (!memory.getNextJob()->isDoingIO() && memory.getNextJob()->getPendingIO()==0)
			memory.deleteFromMemory(memory.getNextJob());
		else {
			cout << "Will kill it" << endl;
			memory.killAfterIO(memory.getNextJob());
		}
	}
	else if(a==7) {			//block
		if (memory.getNextJob()->isDoingIO()) {
			cout << "Doing IO" << endl;
			memory.blockJob();
		}
		else
			if (memory.getNextJob()->getPendingIO()>0) {
				memory.getNextJob()->setBlocked(true);
				swapOutQ.push(memory.getNextJob());
				swapOut(a, p);
				memory.pop();
			}
	}
	runCurrentJob(a, p);
}


bool swapIn(int &a, int p[], PCB *temp, int memoryPos) {
	if (!swappingIn && !swappingOut) {
		siodrum(temp->getJobNumber(), temp->getJobSize(), memoryPos, 0);
		swappingIn = true;
		beingSwapped = temp;
		return true;
	}
	return false;
}

void swapOut(int &a, int p[]) {
	if (!swappingIn && !swappingOut && !swapOutQ.empty()) {
		if ((swapOutQ.front()->isDoingIO() || swapOutQ.front()->getPendingIO()==0 || !swapOutQ.front()->isInMemory()) && !swapOutQ.front()->isTooBig()) {
			swapOutQ.pop();
			return;
		}
		swapOutQ.front()->setBlocked(true);
		siodrum(swapOutQ.front()->getJobNumber(), swapOutQ.front()->getJobSize(), swapOutQ.front()->getMemoryPos(), 1);
		swappingOut = true;
		memory.deleteFromMemory(swapOutQ.front());
		if (swapOutQ.front()->isTooBig()) {
			swapOutQ.front()->setTooBig(false);
			swapOutQ.front()->setBlocked(false);
			LTS.push_back(swapOutQ.front());
		}
		swapOutQ.pop();
	}
}

void runCurrentJob(int &a, int p[]) {
	if (!memory.isEmpty()) {
		//cout << memory.getNextJob()->getJobNumber() << endl << memory.getNextJob()->getMemoryPos() << endl << memory.getNextJob()->getJobSize() << endl;
		p[2] = memory.getNextJob()->getMemoryPos();
		p[3] = memory.getNextJob()->getJobSize();
		p[4] = 1;
		a = 2;
		memory.getNextJob()->setPrevClock(p[5]);
	}
	else
		runFromLTS(a, p);
}	

void runFromLTS(int &a, int p[]) {
	cout << "Run from LTS " << LTS.size()<< endl;
	swapFromLTS(a, p);
	a = 1;
}

void swapFromLTS(int &a, int p[]) {
	if (!LTS.empty()) {
		PCB* temp = LTS.back();
		int memoryPos = memory.findMemPos(temp);
		if (memoryPos != -1) {
			if (swapIn(a, p, temp, memoryPos))
				LTS.pop_back();
		}
	}
}

void runIO(int &a, int p[]) {
	std::sort(IO.begin(), IO.end(), memory.sortIO);
	cout << IO.size() << " " << doingIO << endl;
	if (!IO.empty() && !doingIO) {
		if (IO.back()->isInMemory()) {
			doingIO = true;
			IO.back()->setDoingIO(true);
			memory.setJobDoingIO(IO.back());
			siodisk(IO.back()->getJobNumber());
			IO.pop_back();
		}
		else {
			int memoryPos = memory.findMemPos(IO.back());
			if (memoryPos != -1) 
				swapIn(a, p, IO.back(), memoryPos);
			else {
				if (swapOutQ.empty() && !swappingIn && !swappingOut) {
					PCB* temp = memory.findLargestJob();
					cout << "Swapout #" << temp->getJobNumber() << "  " << memoryPos << endl;
					swapOutQ.push(temp);
					swapOut(a, p);

				}
			}
		}
	}
}










extern "C" FILE* __cdecl __iob_func()
{
	struct _iobuf_VS2012 { // ...\Microsoft Visual Studio 11.0\VC\include\stdio.h #56
		char *_ptr;
		int   _cnt;
		char *_base;
		int   _flag;
		int   _file;
		int   _charbuf;
		int   _bufsiz;
		char *_tmpfname;
	};
	// VS2015 has only FILE = struct {void*}

	int const count = sizeof(_iobuf_VS2012) / sizeof(FILE);

	//// stdout
	//return (FILE*)(&(__acrt_iob_func(1)->_Placeholder) - count);

	// stderr
	return (FILE*)(&(__acrt_iob_func(2)->_Placeholder) - 2 * count);
}