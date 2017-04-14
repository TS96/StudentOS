#include "PCB.h"



PCB::PCB(int jobN, int pri, int jobS, int maxTime, int memPos) : jobNumber(jobN), priority(pri), jobSize(jobS), maxCPUTime(maxTime), memoryPos(memPos)
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

int PCB::getMemoryPos() {
	return memoryPos;
}

void PCB::setMemoryPos(int pos) {
	memoryPos = pos;
}

