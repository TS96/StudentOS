#include "PCB.h"



PCB::PCB(int jobN, int pri, int jobS, int maxTime, int memPos) : jobNumber(jobN), priority(pri), jobSize(jobS), maxCPUTime(maxTime), memoryPos(memPos)
{
	cpuTime = 0;
	doingIO = false;
	blocked = false;
}
PCB::PCB() {
	jobNumber = priority = jobSize = maxCPUTime = memoryPos = -1;
	cpuTime = 0;
	doingIO = blocked = false;
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

int PCB::getCPUTime() {
	return cpuTime;
}

void PCB::addCPUTime(int n) {
	cpuTime += n;
}

bool PCB::isBlocked() {
	return blocked;
}

bool PCB::isDoingIO() {
	return doingIO;
}

void PCB::setBlocked(bool status) {
	blocked = status;
}

void PCB::setDoingIO(bool status) {
	doingIO = status;
}

