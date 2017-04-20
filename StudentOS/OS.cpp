#include <filesystem>
#include "PCB.h"
#include "Memory.h"
#include <iostream>
#include <queue>


Memory memory;
PCB beingSwapped;
queue<PCB> LTS;
queue<PCB*> IO;
queue<PCB> swapOutQ;
bool swappingIn = false;
bool swappingOut = false;
bool doingIO = false;
void siodisk(int jobnum);
void siodrum(int jobnum, int jobsize, int coreaddress, int direction);
void ontrace(); // called without arguments
void offtrace(); // called without arguments 
void runFromLTS(int &, int []);
void runCurrentJob(int &, int []);
void runIO(int &, int[]);
bool swapIn(int &, int[], PCB &, int);
void swapOut(int &, int[], PCB &);





void startup()
{
	ontrace();
	// Allows initialization of static system variables declared above.
	// Called once at start of the simulation.
}

// INTERRUPT HANDLERS
// The following 5 functions are the interrupt handlers. The arguments 
// passed from the environment are detailed with each function below.
// See RUNNING A JOB, below, for additional information
void Crint(int &a, int p[])
{
	cout << "new job" << endl;
	// Indicates the arrival of a new job on the drum.
	// At call: p [1] = job number
	// p [2] = priority
	// p [3] = job size, K bytes
	// p [4] = max CPU time allowed for job
	// p [5] = current time
	PCB temp(p[1], p[2], p[3], p[4], -1);
	int memoryPos = memory.findMemPos(temp);
	if (memoryPos != -1) {
		swapIn(a, p, temp, memoryPos);
	}
	else {
		LTS.push(temp);
	}
	runCurrentJob(a, p);
}
void Dskint(int &a, int p[])
{
	cout << "disk interrupt" << " " << a << endl;
	// Disk interrupt.
	// At call: p [5] = current time
	doingIO = false;
	if (memory.shouldKill()) {
		cout << "killed it" << endl;
		memory.deleteFromMemory(*IO.front());
	}
	else if (IO.front()->isBlocked()) {
		IO.front()->setBlocked(false);
		IO.front()->setDoingIO(false);
		memory.push(*IO.front());
	}
	else
		IO.front()->setDoingIO(false);
	IO.pop();
	runCurrentJob(a, p);
	runIO(a, p);
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
	runCurrentJob(a, p);
	// Drum interrupt.
	// At call: p [5] = current time
}
void Tro(int &a, int p[])
{
	// Timer-Run-Out.
	// At call: p [5] = current time
	memory.getNextJob().addCPUTime(p[4]);
	if (memory.getNextJob().getCPUTime() >= memory.getNextJob().getMaxCPUTime()) {
		cout << "ran out of time" << endl;
		memory.deleteFromMemory(memory.getNextJob());
		runCurrentJob(a,p);
	}
	else
		a = 2;
}
void Svc(int &a, int p[])
{
	cout << "svc request" << " " << a << endl;
	if (a == 6) {		//needs IO
		IO.push(&memory.getNextJob());
		runIO(a, p);
		runCurrentJob(a, p);
	}
	else if (a == 5) {		//terminated
		if (!memory.getNextJob().isDoingIO())
			memory.deleteFromMemory(memory.getNextJob());
		else
			memory.killAfterIO(memory.getNextJob());
		runCurrentJob(a, p);
	}
	else if(a==7) {			//block
		if (memory.getNextJob().isDoingIO()) {
			cout << "Doing IO" << endl;
			memory.blockJob();
		}
		runCurrentJob(a, p);
	}
}


bool swapIn(int &a, int p[], PCB &temp, int memoryPos) {
	if (!swappingIn && !swappingOut) {
		siodrum(temp.getJobNumber(), temp.getJobSize(), memoryPos, 0);
		swappingIn = true;
		beingSwapped = temp;
		return true;
	}
	return false;
}

void swapOut(int &a, int p[], PCB &pcb) {
	if (!swappingIn && !swappingOut) {
		pcb.setBlocked(true);
		siodrum(pcb.getJobNumber(), pcb.getJobSize(), pcb.getMemoryPos(), 1);
		swappingOut = true;
		memory.deleteFromMemory(pcb);
	}
	else {
		swapOutQ.push(pcb);
		memory.pop();
	}
}

void runCurrentJob(int &a, int p[]) {
	if (!memory.isEmpty()) {
		p[2] = memory.getNextJob().getMemoryPos();
		p[3] = memory.getNextJob().getJobSize();
		p[4] = 1;
		a = 2;
	}
	else
		runFromLTS(a, p);
}

void runFromLTS(int &a, int p[]) {
	cout << "Run from LTS" << endl;
	if (!LTS.empty()) {
		PCB temp = LTS.front();
		int memoryPos = memory.findMemPos(temp);
		if (memoryPos != -1) {
			swapIn(a, p, temp, memoryPos);
			LTS.pop();
		}
		a = 1;
	}
	else
		a = 1;
}

void runIO(int &a, int p[]) {
	if (!IO.empty() && !doingIO) {
		doingIO = true;
		IO.front()->setDoingIO(true);
		memory.setJobDoingIO(*IO.front());
		siodisk(IO.front()->getJobNumber());
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