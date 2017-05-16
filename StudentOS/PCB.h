/*
Tarek Saidee
CISC 3320
*/

#ifndef PCB_H
#define PCB_H

class PCB
{

	int jobNumber;		//stores the job #
	int priority;		//stores the priority
	int jobSize;		//stores the job size
	int maxCPUTime;		//stores the max CPU time
	int memoryPos;		//stores its position in memory
	int cpuTime;		//total time running 
	int pendingIO;		//number of pending IO requests
	bool blocked;		//if the job is blocked
	bool doingIO;		//if it's doing IO
	bool killAfterIO;	//if it should be killed after finishing IO
	bool inMemory;		//if the job is in memory
	bool tooBig;		//if the job should be swapped out to make room
	int prevClock;		//time it last ran of time
	int CPUTimeLeft;	//stores how much max CPU time is left
	bool SwapOut;		//whether it should be swapped out

public:
	PCB(int , int , int , int , int);
	PCB();
	~PCB();
	int getJobNumber();
	int getPriority();
	int getJobSize();
	int getMaxCPUTime();
	int getMemoryPos();
	int getCPUTime();
	void addCPUTime(int);
	void setMemoryPos(int);
	bool isBlocked();
	bool isDoingIO();
	void setBlocked(bool);
	void setDoingIO(bool);
	void setKillAfterIO(bool);
	bool shouldKill();
	int getPendingIO();
	void incrementPendingIO();
	bool isInMemory();
	void setInMemory(bool);
	bool isTooBig();
	void setTooBig(bool);
	int getPrevClock();
	void setPrevClock(int);
	int getCPUTimeLeft();
	void setToSwapOut(bool);
	bool toSwapOut();
	bool operator() (PCB*, PCB*) const;
};

#endif


