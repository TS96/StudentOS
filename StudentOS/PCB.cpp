/*
Tarek Saidee
CISC 3320
*/

#include "PCB.h"
#include "Memory.h"


//constructor
PCB::PCB(int jobN, int pri, int jobS, int maxTime, int memPos) : jobNumber(jobN), priority(pri), jobSize(jobS), maxCPUTime(maxTime), memoryPos(memPos), CPUTimeLeft(maxCPUTime)
{
	cpuTime = 0;
	doingIO = false;
	blocked = false;
	killAfterIO = false;
	pendingIO = 0;
	inMemory = false;
	tooBig = false;
	SwapOut = false;
	prevClock = 0;
}

//another constructor
PCB::PCB() {
	jobNumber = priority = jobSize = maxCPUTime = memoryPos = -1;
	cpuTime = 0;
	doingIO = blocked = false;
	killAfterIO = false;
	pendingIO = 0;
	inMemory = false;
	SwapOut = false;
	tooBig = false;
	prevClock = 0;
	CPUTimeLeft = 0;
}

//destructor
PCB::~PCB() {

}


//returns the job number
int PCB::getJobNumber() {
	return jobNumber;
}

//returns the job size
int PCB::getJobSize() {
	return jobSize;
}

//returns the max CPU time
int PCB::getMaxCPUTime() {
	return maxCPUTime;
}

//returns the priority
int PCB::getPriority() {
	return priority;
}

//returns the memory position
int PCB::getMemoryPos() {
	return memoryPos;
}

//sets the memory position
void PCB::setMemoryPos(int pos) {
	memoryPos = pos;
}


//return total CPU time
int PCB::getCPUTime() {
	return cpuTime;
}

//increases the total CPU time
void PCB::addCPUTime(int n) {
	cpuTime += n;
	CPUTimeLeft -= n;
}

//checks if it's blocked
bool PCB::isBlocked() {
	return blocked;
}

//checks if it's doing IO
bool PCB::isDoingIO() {
	return doingIO;
}

//block or unblock job
void PCB::setBlocked(bool status) {
	blocked = status;
}

//set it doing or not doing IO
void PCB::setDoingIO(bool status) {
	doingIO = status;
	if (!status)
		pendingIO--;
}

//set if it should be killed after done with IO
void PCB::setKillAfterIO(bool status) {
	killAfterIO = status;
}

//check if it should be killed
bool PCB::shouldKill() {
	return killAfterIO;
}

//return # of pending IO requests
int PCB::getPendingIO() {
	return pendingIO;
}

//add a pending IO request
void PCB::incrementPendingIO() {
	pendingIO++;
}

//check if it's in memory
bool PCB::isInMemory() {
	return inMemory;
}

//set if it's in memory or not
void PCB::setInMemory(bool status) {
	inMemory = status;
}

//check if it's too big and needs to be swapped out to make room
bool PCB::isTooBig() {
	return tooBig;
}

//set the toobig flag
void PCB::setTooBig(bool status) {
	tooBig = status;
}


//set the last time it ran out of time
void PCB::setPrevClock(int n) {
	prevClock = n;
}

//return the last time it ran out of time
int PCB::getPrevClock() {
	return prevClock;
}

//returns how much CPU time is left
int PCB::getCPUTimeLeft() {
	return CPUTimeLeft;
}

//check if it should be swapped out
bool PCB::toSwapOut() {
	return SwapOut;
}

//set it to be swapped out
void PCB::setToSwapOut(bool status) {
	SwapOut = status;
}

//overrides the () operator to be used as the compare function in LTS' priority queue
bool PCB::operator() (PCB* lhs, PCB* rhs) const{
	return Memory::sortByPCBSize(lhs, rhs);				//sort by PCBSize dilation = 1.32, response time 2.5, jobs 448
														//sort by remaining time dilation = 1.47, response time 2.3, jobs 450
}