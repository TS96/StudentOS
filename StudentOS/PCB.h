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
	bool blocked;
	bool doingIO;

public:
	PCB(int , int , int , int , int);
	PCB();
	int getJobNumber();
	int getPriority();
	int getJobSize();
	int getMaxCPUTime();
	int getMemoryPos();
	int getCPUTime();
	void addCPUTime(int);
	void setMemoryPos(int);
};

#endif


