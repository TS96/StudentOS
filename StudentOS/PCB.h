#ifndef PCB_H;
#define PCB_H

class PCB
{

	int jobNumber;
	int priority;
	int jobSize;
	int maxCPUTime;

public:
	PCB(int , int , int , int );

	int getJobNumber();
	int getPriority();
	int getJobSize();
	int getMaxCPUTime();
};

#endif


