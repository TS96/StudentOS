#ifndef PCB_H
#define PCB_H

class PCB
{

	int jobNumber;
	int priority;
	int jobSize;
	int maxCPUTime;
	int memoryPos;

public:
	PCB(int , int , int , int , int);

	int getJobNumber();
	int getPriority();
	int getJobSize();
	int getMaxCPUTime();
	int getMemoryPos();
	void setMemoryPos(int);
};

#endif


