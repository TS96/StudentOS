#include <filesystem>
#include "PCB.h"
#include "Memory.h"
#include <iostream>
#include <queue>


std::queue<PCB> q;
bool io = false;
bool blocked = false;
Memory memory;
PCB currentJob;
void siodisk(int jobnum);
void siodrum(int jobnum, int jobsize, int coreaddress, int direction);
void ontrace(); // called without arguments
void offtrace(); // called without arguments 

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
	if (memory.insertNewJob(temp)) {
		memory.printFST();
		q.push(temp);
		siodrum(temp.getJobNumber(), temp.getJobSize(), temp.getMemoryPos(), 0);
	}
}
void Dskint(int &a, int p[])
{
	cout << "disk interrupt" << " " << a << endl;
	// Disk interrupt.
	// At call: p [5] = current time
	/*if (!q.empty() && a != 2) {
		p[2] = q.front().getMemoryPos();
		p[3] = q.front().getJobSize();
		p[4] = 4;
		currentJob = q.front();
		q.pop();
		a = 2;
	}
	else*/
	blocked = false;
	io = false;
		a = 2;
}
void Drmint(int &a, int p[])
{
	cout << "drum interrupt" << " " << a << endl;
	if (blocked) {
		siodrum(q.front().getJobNumber(), q.front().getJobSize(), q.front().getMemoryPos(), 0);
		memory.insertNewJob(q.front());
		a = 1;
		blocked = false;
		return;
	}
		if (!q.empty() && a != 2) {
			if (q.front().getMemoryPos() == -1)
				memory.insertNewJob(q.front());
			p[2] = q.front().getMemoryPos();;
			p[3] = q.front().getJobSize();
			p[4] = 1;
			currentJob = q.front();
			q.pop();
			a = 2;
		}
		else
			a = 1;
	// Drum interrupt.
	// At call: p [5] = current time
}
void Tro(int &a, int p[])
{
	// Timer-Run-Out.
	// At call: p [5] = current time
	currentJob.addCPUTime(p[4]);
	if (currentJob.getCPUTime() >= currentJob.getMaxCPUTime()) {
		cout << "ran out of time" << endl;
		//siodrum(currentJob.getJobNumber(), currentJob.getJobSize(), currentJob.getMemoryPos(), 1);
		memory.deleteFromMemory(currentJob);
		if (!q.empty()) {
			currentJob = q.front();
			q.pop();
			a = 2;
		}
		else
			a = 1;
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
		//q.push(currentJob);
		siodisk(currentJob.getJobNumber());
		io = true;
		a = 2;
	}
	else if (a == 5) {
		memory.deleteFromMemory(currentJob);
		io = false;
		a = 1;
	}
	else if(a==7) {
		cout << "wating" << endl;
		if (!io) {
			blocked = true;
			siodrum(currentJob.getJobNumber(), currentJob.getJobSize(), currentJob.getMemoryPos(), 1);
			q.push(currentJob);
			memory.deleteFromMemory(currentJob);
		}
		a = 1;

	}
}


FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}