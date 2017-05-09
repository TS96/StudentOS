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
void runIO(int &, int[], bool);
void freeMemory(int &, int[], int);


//Used to initilize objects and variables.
void startup()
{
	//ontrace();
}

/*
	When a new job comes in, Crint gets called and p[] gets filled with 
	information about that job. p[2] = priority, p[3] = job size in bytes,
	p[4] = max CPU Time, p[5] = current time. Here, I either swap it into memory
	if there's space and the swapper is free or I push it onto the LTS. 
	At the end you run the next job if there's one available.
*/
void Crint(int &a, int p[])
{
	cout << "new job #" << p[1]<< endl;
	PCB* temp = new PCB(p[1], p[2], p[3], p[4], -1);
	int memoryPos = memory.findMemPos(temp);
	if (memoryPos != -1 && swapper.swapIn(a, p, temp, memoryPos)) {
		cout << "swapped in new job #" << p[1] << endl;
	}
	else 
		swapper.addToLTS(temp);
	runCurrentJob(a, p);
}


/*
	An interrupt handler for when a job finishes IO. If it was terminated while
	doing IO, then you just delete it from memory. If it was blocked, 
	then you pushed it back into the STS because it's still in memory
	and of course you unblock it and indicate that it's not doing IO.
	At the end, you try to start IO for a another job and run the next job
	if there's any for both.
*/
void Dskint(int &a, int p[])
{
	cout << "disk interrupt" << endl;
	doingIO = false;
	if (memory.getJobDoingIO()->shouldKill()) {
		cout << "killed it" << endl;
		memory.deleteFromMemory(memory.getJobDoingIO());
	}
	else {
		memory.getJobDoingIO()->setDoingIO(false);
		if (memory.getJobDoingIO()->isBlocked()) {
			memory.getJobDoingIO()->setBlocked(false);
			memory.push(memory.getJobDoingIO());
		}
	}
	runIO(a, p, false);
	runCurrentJob(a, p);
}

/*
	An interrupt handler for when a job gets swapped in or out
	of memory. If a job was being swapped in, then it gets inserted into 
	my representation of memory. At the end, I try to run IO first because
	if the disk is idle, then you want to swapin a new job for IO asap. Then 
	I run next job and if none is avaiable then I'd want to swapin a new one asap.
	Then I try to swap in a job from the LTS even though it might not currently be needed
	but that keeps the drum busy. I also check if the swapoutQ has something that needs to be
	swapped out.
*/
void Drmint(int &a, int p[])
{
	cout << "drum interrupt" << " " << a << endl;
	if (swapper.isSwappingIn()) {
		memory.insertNewJob(swapper.getJobBeingSwapped());
		swapper.setSwappingIn(false);
	}
	else 
		swapper.setSwappingOut(false);
	runIO(a, p, false);
	runCurrentJob(a, p);
	swapper.swapFromLTS(a, p, memory);
	swapper.swapOut(a, p, memory);
}

/*
	Interrupt handler for when a job runs out of time.
	Total runtime gets updated then if it's bigger or equal to 
	max CPU time, the job gets killed if it's not doing IO or has pending IO, 
	if it is then you mark it to be killed after it's done with IO. If it hasn't
	exceded its max time then nothing happens and you run next job normally.
*/
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


/*
Service interrupt handler for termination, IO request or be blocked.
For termination, check if it has no pending or currently doing IO
and kill it if not or mark to be killed when it's done. For IO requests,
you call run IO with true as a parameter to mark it as a new IO request.
For blocking, if it's doing IO then you block it in memory, if not you check 
if it has any pending IO and if so you pop it from the STS (but stays in memory
until it's swapped out) and add it to the swapoutQ and try to swapout because 
it's currently just taking up space in memory.
*/
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

		case 6:					//needs IO
			runIO(a, p, true);
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

/*
	Gets called when I want to run the next job. Checks if memory is empty and 
	if it's not, then it gets the next job from the STS and runs it. If it is empty,
	then it calls the LTS to check if it can run something from there.
*/
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


/*
	Handles all running IO. If it's a new request, then it adds it to the 
	IO queue. The IO queue gets sorted based on my algorithm (not really a queue)
	then it checks if it's empty or the disk is busy. If they're both false then
	if the top of the queue is in memory, then you just start IO on it. If not, then
	you need to find a place for it in memory and swap it in and if memory is full
	then you need to free it in order to keep the disk busy at all times.
*/
void runIO(int &a, int p[], bool newRequest) {
	if (newRequest) {
		memory.getNextJob()->incrementPendingIO();
		IO.push_back(memory.getNextJob());
	}
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
				freeMemory(a, p, memoryPos);
			}
		}
	}
}

/*
	FreeMemory finds the largest job in memory and tries to swap it out only if the 
	drum is not busy because other wise it might get pushed to the swapoutQ but
	only gets its turn after you don't need to free memory anymore. It doesn't check
	if the job is doing IO or has pending IO because the function only gets called
	when no job in memory can do IO.
*/
void freeMemory(int &a, int p[], int memoryPos) {
	if (swapper.swapOutQEmpty() && !swapper.isSwappingIn() && !swapper.isSwappingOut()) {
		PCB* temp = memory.findLargestJob();
		cout << "Swapout #" << temp->getJobNumber() << "  " << memoryPos << endl;
		swapper.addToSwapOutQ(temp);
		swapper.swapOut(a, p, memory);
	}
}








/*
	I had to add this function which I got from the good people at
	StackOverflow because the SOS file was compiled with an old compiler
	and the internal implementation of some functions was changed afterwards
	so this was the only way I could get it to work without having to recompile
	the object file with a newer compiler.
*/
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