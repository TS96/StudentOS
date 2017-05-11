#include "Memory.h"
#include <algorithm>
#include <iostream>


/*
	can decrease response time to 1749 if I sort IO maxcputime but dilation would be 1.55 with LTS by remainingtime
	sort IO by maxcputime dilation = 1.30, response = 2.3, jobs = 441 with LTS by size
*/
Memory::Memory()
{
	FST.push_back(pair<int, int>(0, 100));
}

//sorts the FST by size in ascending order
bool Memory::sortBySize(pair<int, int> left, pair<int, int> right) {
	return left.second < right.second;
}

//sorts the FST by address in ascending order
bool Memory::sortByAddress(pair<int, int> left, pair<int, int> right) {
	return left.first < right.first;
}

//sorts PCBs by max cpu time in descending order
bool Memory::sortByMaxCPUTime(PCB* left, PCB* right) {
	return left->getMaxCPUTime() > right->getMaxCPUTime();
}


////sorts PCBs by max cpu time in ascending order
bool Memory::sortByLeastCPUTime(PCB* left, PCB* right) {
	return left->getMaxCPUTime() < right->getMaxCPUTime();
}

//sorts PCBs by job size in descending order
bool Memory::sortByPCBSize(PCB* left, PCB* right) {
	return left->getJobSize() > right->getJobSize();
}

//sorts PCBs by job size in ascending order
bool Memory::sortByBiggestSize(PCB* left, PCB* right) {
	return left->getJobSize() < right->getJobSize();
}

//sorts PCBs by remaining time left in descending order
bool Memory::sortByRemainingTime(PCB* left, PCB* right) {
	return left->getCPUTimeLeft() > right->getCPUTimeLeft();
}

//sorts PCBs by remaining time left in ascending order
bool Memory::sortByBiggestRemainingTime(PCB* left, PCB* right) {
	return left->getCPUTimeLeft() < right->getCPUTimeLeft();
}

//sorts IO first by putting the jobs in memory ahead then sorts the rest by remaining time
bool Memory::sortIO(PCB* left, PCB* right) {
	if (left->isInMemory() && right->isInMemory())
		return sortByRemainingTime(left, right);
	else
		return left->isInMemory() < right->isInMemory();
}

/*
	inserts a new job in memory but first it checks if the memory position that was assigned to the job matches what's available in 
	memory. If the memory position starts at the beginning of the FST entry, then you simply update that entry by subtracting the size
	of the new job. Otherwise the FST entry has to split in two entries because the new job goes between them. Then if the job isn't blocked
	(waiting to finish IO) then it can be added to the STS to be run at some point otherwise it just waits in memory for IO.
*/
bool Memory::insertNewJob(PCB *newJob) {
	int i = findSpot(newJob);
	if (i == -1)
		return false;
	while (!(FST[i].first <= newJob->getMemoryPos()) || !(FST[i].first + FST[i].second >= newJob->getMemoryPos() + newJob->getJobSize()))
		i++;
	pair<int, int> oldSpace = FST[i];
	if (oldSpace.first == newJob->getMemoryPos()) {
			if (oldSpace.second - newJob->getJobSize() != 0) {
				FST[i].first = oldSpace.first + newJob->getJobSize();
				FST[i].second = oldSpace.second - newJob->getJobSize();
			}
			else {
				FST.erase(FST.begin() + i);
			}
	}
	else {
		int orgSize = FST[i].second;
		FST[i].second = newJob->getMemoryPos() - FST[i].first;
		pair<int, int> p;
		p.first = newJob->getMemoryPos() + newJob->getJobSize();
		p.second = orgSize - FST[i].second - newJob->getJobSize();
		if (p.second > 0)
			FST.push_back(p);
	}

	newJob->setInMemory(true);
	if (!newJob->isBlocked())
		jobs.push_back(newJob);
	std::sort(jobs.begin(), jobs.end(), sortByMaxCPUTime);
	mergeAdjacentSpaces();
	return true;
}


//merges the adjacent entries in the FST
void Memory::mergeAdjacentSpaces() {
	if (FST.size() < 2)
		return;
	std::sort(FST.begin(), FST.end(), sortByAddress);
	for (size_t  i = 0; i < FST.size() - 1; i++) {
		if (FST[i].first + FST[i].second == FST[i + 1].first) {
			FST[i].second += FST[i + 1].second;
			FST.erase(FST.begin() + i + 1);
			i = -1;
		}
	}
	std::sort(FST.begin(), FST.end(), sortBySize);
}

/*
	deletes a job from memory. First it gives its space back to the FST and merges the adjacent spaces
	the if it's not meants to be killed after IO, doesn't have any pending IO and isn't going to be swapped
	out to free memory, it gets deleted from the heap and popped from the STS. Other wise if it should be killed 
	after IO, then it gets deleted straight away from the heap without being popped from the STS because it was
	never there.
*/
bool Memory::deleteFromMemory(PCB *pcb) {
	if (pcb->getMemoryPos() == -1)
		return false;
	FST.push_back(pair<int, int>(pcb->getMemoryPos(), pcb->getJobSize()));
	mergeAdjacentSpaces();
	pcb->setMemoryPos(-1);
	pcb->setInMemory(false);
	if (!pcb->shouldKill() && !(pcb->getPendingIO() > 0) && !pcb->isTooBig()) {
		pop();
		if(!pcb->toSwapOut())
			delete pcb;
	}
	else 
		if (pcb->shouldKill()) {
			delete pcb;
		}
	printFST();
	return true;
}

//finds the index of a free space in memory
int Memory::findSpot(PCB* job) {
	for (size_t  i = 0; i < FST.size(); i++)
		if (FST[i].second >= job->getJobSize()) {
			return i;
		}
	return -1;
}

//finds the position in memory
int Memory::findMemPos(PCB* p) {
	int i = findSpot(p);
	if (i != -1) {
		p->setMemoryPos(FST[i].first);
		return FST[i].first;
	}
	return i;
}

//prints the FST for debugging purposes
void Memory::printFST() {
	for (pair<int, int> p : FST) {
		cout << p.first << " " << p.second << endl;
	}
}

//returns the next job to be run in the STS
PCB * Memory::getNextJob() {
	if (jobs.empty())
		NULL;
	return jobs.back();
}

//checks if the STS is empty
bool Memory::isEmpty() {
	return jobs.empty();
}

//pops the STS
void Memory::pop() {
	jobs.pop_back();
}

//pushes a new job into the STS then it sorts it
void Memory::push(PCB* p) {
	jobs.push_back(p);
	std::sort(jobs.begin(), jobs.end(), sortByMaxCPUTime);
}


//returns the number of jobs in the STS
int Memory::getCount() {
	return jobs.size();
}


//blocks a job by removing it from the STS without deleting it from memory
void Memory::blockJob() {
	jobs.back()->setBlocked(true);
	jobs.pop_back();
}

//marks a job to be killed after it's done IO and it also removes it from the STS because it's done with the CPU
void Memory::killAfterIO(PCB* p) {
	p->setKillAfterIO(true);
	pop();
}

//updates the job currently doing IO
void Memory::setJobDoingIO(PCB* p) {
	jobDoingIO = p;
}


//returns the job currently doing IO
PCB* Memory::getJobDoingIO() {
	return jobDoingIO;
}

//finds the largest job currently in memory but not doing IO and returns it
PCB* Memory::findLargestJob() {
	std::sort(jobs.begin(), jobs.end(), sortByBiggestSize);			//sort by leastcputime dilation =1.42, response = 2.1, jobs = 446
	PCB* temp = jobs.back();
	jobs.pop_back();
	temp->setTooBig(true);
	std::sort(jobs.begin(), jobs.end(), sortByMaxCPUTime);
	return temp;
}