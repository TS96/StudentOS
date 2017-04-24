#include "Memory.h"
#include <algorithm>
#include <iostream>


Memory::Memory()
{
	FST.push_back(pair<int, int>(0, 100));
}

bool Memory::sortBySize(pair<int, int> left, pair<int, int> right) {
	return left.second < right.second;
}

bool Memory::sortByAddress(pair<int, int> left, pair<int, int> right) {
	return left.first < right.first;
}

bool Memory::sortByMaxCPUTime(PCB* left, PCB* right) {
	return left->getMaxCPUTime() > right->getMaxCPUTime();
}

bool Memory::sortByPCBSize(PCB* left, PCB* right) {
	return left->getJobSize() > right->getJobSize();
}

bool Memory::sortIO(PCB* left, PCB* right) {
	if (left->isInMemory() && right->isInMemory())
		return left->getJobSize() > right->getJobSize();
	else
		return left->isInMemory() < right->isInMemory();
}

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

void Memory::mergeAdjacentSpaces() {
	if (FST.size() < 2)
		return;
	std::sort(FST.begin(), FST.end(), sortByAddress);
	for (int i = 0; i < FST.size() - 1; i++) {
		if (FST[i].first + FST[i].second == FST[i + 1].first) {
			FST[i].second += FST[i + 1].second;
			FST.erase(FST.begin() + i + 1);
			i = -1;
		}
	}
	std::sort(FST.begin(), FST.end(), sortBySize);
}

bool Memory::deleteFromMemory(PCB *pcb) {
	if (pcb->getMemoryPos() == -1)
		return false;
	FST.push_back(pair<int, int>(pcb->getMemoryPos(), pcb->getJobSize()));
	mergeAdjacentSpaces();
	pcb->setMemoryPos(-1);
	pcb->setInMemory(false);
	if (!pcb->shouldKill() && !(pcb->getPendingIO() > 0)) {
		pop();
		delete pcb;
	}
	else if (pcb->shouldKill())
		delete pcb;
	printFST();
	return true;
}

int Memory::findSpot(PCB* job) {
	for (int i = 0; i < FST.size(); i++)
		if (FST[i].second >= job->getJobSize()) {
			return i;
		}
	return -1;
}

int Memory::findMemPos(PCB* p) {
	int i = findSpot(p);
	if (i != -1) {
		p->setMemoryPos(FST[i].first);
		return FST[i].first;
	}
	return i;
}

void Memory::printFST() {
	for (pair<int, int> p : FST) {
		cout << p.first << " " << p.second << endl;
	}
}

PCB * Memory::getNextJob() {
	if (jobs.empty())
		NULL;
	return jobs.back();
}

bool Memory::isEmpty() {
	return jobs.empty();
}

void Memory::pop() {
	jobs.pop_back();
}

void Memory::push(PCB* p) {
	jobs.push_back(p);
	std::sort(jobs.begin(), jobs.end(), sortByMaxCPUTime);
}

int Memory::getCount() {
	return jobs.size();
}

void Memory::blockJob() {
	jobs.back()->setBlocked(true);
	jobs.pop_back();
}

void Memory::killAfterIO(PCB* p) {
	p->setKillAfterIO(true);
	pop();
}

void Memory::setJobDoingIO(PCB* p) {
	jobDoingIO = p;
}

PCB* Memory::getJobDoingIO() {
	return jobDoingIO;
}
