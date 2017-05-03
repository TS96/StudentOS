#include <filesystem>
#include "PCB.h"
#include "Memory.h"
#include "Swapper.h"
#include <iostream>
#include <queue>
#include <vector>

Memory memory;
Swapper swapper;
vector<PCB*> IO;

bool doingIO = false;
void siodisk(int jobnum);
void ontrace(); 
void offtrace();
void runCurrentJob(int &, int []);
void runIO(int &, int[]);


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
	if (memoryPos != -1 && swapper.swapIn(a, p, temp, memoryPos)) {
		cout << "swapped in new job #" << p[1] << endl;
	}
	else 
		swapper.addToLTS(temp);
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
	else 
		if (memory.getJobDoingIO()->isBlocked()) {
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
	if (swapper.isSwappingIn()) {
		memory.insertNewJob(swapper.getJobBeingSwapped());
		swapper.setSwappingIn(false);
	}
	else 
		swapper.setSwappingOut(false);
	runIO(a, p);
	runCurrentJob(a, p);
	swapper.swapFromLTS(a, p, memory);
	swapper.swapOut(a, p, memory);
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
			memory.deleteFromMemory(memory.getNextJob());
	}
	runCurrentJob(a,p);
}

void Svc(int &a, int p[])
{
	cout << "svc request" << " " << a << endl;
	switch (a) {
		case 5:					//terimnate
			if (!memory.getNextJob()->isDoingIO() && memory.getNextJob()->getPendingIO() == 0)
				memory.deleteFromMemory(memory.getNextJob());
			else {
				cout << "Will kill it" << endl;
				memory.killAfterIO(memory.getNextJob());
			}
			break;

		case 6: 
			if (a == 6) {		//needs IO
				memory.getNextJob()->incrementPendingIO();
				IO.push_back(memory.getNextJob());
				runIO(a, p);
			}
			break;
	
		case 7:					//block
			if (memory.getNextJob()->isDoingIO()) {
				cout << "Doing IO" << endl;
				memory.blockJob();
			}
			else
				if (memory.getNextJob()->getPendingIO()>0) {
					memory.getNextJob()->setBlocked(true);
					swapper.addToSwapOutQ(memory.getNextJob());
					swapper.swapOut(a, p, memory);
					memory.pop();
				}
			break;
	}
	runCurrentJob(a, p);
}

void runCurrentJob(int &a, int p[]) {
	if (!memory.isEmpty()) {
		p[2] = memory.getNextJob()->getMemoryPos();
		p[3] = memory.getNextJob()->getJobSize();
		p[4] = 1;
		a = 2;
		memory.getNextJob()->setPrevClock(p[5]);
	}
	else
		swapper.runFromLTS(a, p, memory);
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
				swapper.swapIn(a, p, IO.back(), memoryPos);
			else {
				if (swapper.swapOutQEmpty() && !swapper.isSwappingIn() && !swapper.isSwappingOut()) {
					PCB* temp = memory.findLargestJob();
					cout << "Swapout #" << temp->getJobNumber() << "  " << memoryPos << endl;
					swapper.addToSwapOutQ(temp);
					swapper.swapOut(a, p, memory);
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