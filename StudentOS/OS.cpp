#include <filesystem>
#include "PCB.h"
#include "Memory.h"
#include <iostream>
#include <queue>



Memory memory;
PCB beingSwapped;
PCB currentlyIO;
queue<PCB> LTS;
bool swapping = false;
bool swappingOut = false;
void siodisk(int jobnum);
void siodrum(int jobnum, int jobsize, int coreaddress, int direction);
void ontrace(); // called without arguments
void offtrace(); // called without arguments 


void runFromLTS(int &a, int p[]) {
	if (!LTS.empty()) {
		PCB temp = LTS.front();
		int memoryPos = memory.findMemPos(temp);
		if (memoryPos != -1 && !swapping && !swappingOut) {
			siodrum(temp.getJobNumber(), temp.getJobSize(), memoryPos, 0);
			temp.setBlocked(false);
			beingSwapped = temp;
			swapping = true;
			LTS.pop();
		}
		a = 1;
	}
	else
		a = 1;
}


void runCurrentJob(int &a, int p[]) {
	if (memory.getCount() != 0) {
		p[2] = memory.getNextJob().getMemoryPos();
		p[3] = memory.getNextJob().getJobSize();
		p[4] = 1;
		a = 2;
	}
	else
		runFromLTS(a, p);
}





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
	if (memoryPos != -1 && !swapping) {
		siodrum(temp.getJobNumber(), temp.getJobSize(), memoryPos, 0);
		swapping = true;
		beingSwapped = temp;
		if (memory.getCount() > 0) {
			runCurrentJob(a, p);
		}
	}
	else {
		LTS.push(temp);
		runCurrentJob(a, p);
	}
}
void Dskint(int &a, int p[])
{
	cout << "disk interrupt" << " " << a << endl;
	// Disk interrupt.
	// At call: p [5] = current time
	if (memory.jobToBeKilled().getJobNumber() == currentlyIO.getJobNumber()) {
		cout << "killed it" << endl;
		memory.deleteFromMemory(memory.jobToBeKilled());
	}
	if (memory.getCount() != 0) {
		memory.getNextJob().setDoingIO(false);
		memory.getNextJob().setBlocked(false);
		a = 2;
	}
	else
		a = 1;
}
void Drmint(int &a, int p[])
{
	cout << "drum interrupt" << " " << a << endl;
	if (swapping) {
		memory.insertNewJob(beingSwapped);
		runCurrentJob(a, p);
		swapping = false;
	}
	else {
		swappingOut = false;
		if (memory.isEmpty()) {
			runFromLTS(a, p);
		}
		else {
			runCurrentJob(a, p);
		}
	}
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
		if (!memory.isEmpty()) {
			a = 2;
		}
		else {
			cout << "Waiting" << endl;
			a = 1;
		}
	}
	else
		a = 2;
}
void Svc(int &a, int p[])
{
	cout << "svc request" << " " << a << endl;
	// Supervisor call from user program.
	// At call: p [5] = current time
	// a = 5 => job has terminated
	// a = 6 => job requests disk i/o
	// a = 7 => job wants to be blocked until all its pending
	// I/O requests are completed
	if (a == 6) {
		currentlyIO = memory.getNextJob();
		siodisk(memory.getNextJob().getJobNumber());
		memory.getNextJob().setDoingIO(true);
		a = 2;
	}
	else if (a == 5) {
		if (!memory.getNextJob().isDoingIO())
			memory.deleteFromMemory(memory.getNextJob());
		else
			memory.killAfterIO(memory.getNextJob());
		runCurrentJob(a, p);
	}
	else if(a==7) {
		cout << "wating" << endl;
		if (!memory.getNextJob().isDoingIO()) {
			memory.getNextJob().setBlocked(true);
			siodrum(memory.getNextJob().getJobNumber(), memory.getNextJob().getJobSize(), memory.getNextJob().getMemoryPos(), 1);
			swappingOut = true;
			LTS.push(memory.getNextJob());
			memory.deleteFromMemory(memory.getNextJob());
			runCurrentJob(a,p);
		}
		else {
			if (memory.getCount() == 1)
				runFromLTS(a,p);
			else {
				memory.blockJob();
				runCurrentJob(a,p);
			}
		}

	}
}



FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}