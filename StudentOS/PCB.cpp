#include "PCB.h"



PCB::PCB(int jobN, int pri, int jobS, int maxTime) : jobNumber(jobN), priority(pri), jobSize(jobS), maxCPUTime(maxTime)
{
	
}

int PCB::getJobNumber() {
	return jobNumber;
}

int PCB::getJobSize() {
	return jobSize;
}

int PCB::getMaxCPUTime() {
	return maxCPUTime;
}

int PCB::getPriority() {
	return priority;
}

