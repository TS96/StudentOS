#ifndef PCB_H
#define PCB_H

class PCB
{

	int jobNumber;
	int priority;
	int jobSize;
	int maxCPUTime;
	int memoryPos;
	int cpuTime;
	int pendingIO;
	bool blocked;
	bool doingIO;
	bool killAfterIO;
	bool inMemory;

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
};

#endif


