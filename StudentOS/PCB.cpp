#include "PCB.h"



PCB::PCB(int jobN, int pri, int jobS, int maxTime, int memPos) : jobNumber(jobN), priority(pri), jobSize(jobS), maxCPUTime(maxTime), memoryPos(memPos)
{
	cpuTime = 0;
	doingIO = false;
	blocked = false;
	killAfterIO = false;
	pendingIO = 0;
	inMemory = false;
	tooBig = false;
	prevClock = 0;
}
PCB::PCB() {
	jobNumber = priority = jobSize = maxCPUTime = memoryPos = -1;
	cpuTime = 0;
	doingIO = blocked = false;
	killAfterIO = false;
	pendingIO = 0;
	inMemory = false;
	tooBig = false;
	prevClock = 0;
}

PCB::~PCB() {

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
	if (!status)
		pendingIO--;
}

void PCB::setKillAfterIO(bool status) {
	killAfterIO = status;
}

bool PCB::shouldKill() {
	return killAfterIO;
}

int PCB::getPendingIO() {
	return pendingIO;
}

void PCB::incrementPendingIO() {
	pendingIO++;
}

bool PCB::isInMemory() {
	return inMemory;
}

void PCB::setInMemory(bool status) {
	inMemory = status;
}

bool PCB::isTooBig() {
	return tooBig;
}

void PCB::setTooBig(bool status) {
	tooBig = status;
}

void PCB::setPrevClock(int n) {
	prevClock = n;
}

int PCB::getPrevClock() {
	return prevClock;
}

