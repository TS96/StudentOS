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


bool Memory::insertNewJob(PCB &newJob) {
	if (findSpot(newJob.getJobSize()) != -1) {
		int i = findSpot(newJob.getJobSize());
		pair<int, int> oldSpace = FST[i];
		if (oldSpace.second - newJob.getJobSize() != 0) {
			FST[i].first = oldSpace.first + newJob.getJobSize();
			FST[i].second = oldSpace.second - newJob.getJobSize();

		}
		else
			FST.erase(FST.begin() + i);
		newJob.setMemoryPos(oldSpace.first);
		jobs.push(newJob);
	}
	else 
		return false;
	std::sort(FST.begin(), FST.end(), sortBySize);
	return true;
}

void Memory::mergeAdjacentSpaces() {
	std::sort(FST.begin(), FST.end(), sortByAddress);
	for (int i = 0; i < FST.size() - 1; i++) {
		if (FST[i].first + FST[i].second == FST[i + 1].first) {
			FST[i].second += FST[i + 1].second;
			FST.erase(FST.begin() + i + 1);
		}
	}
	std::sort(FST.begin(), FST.end(), sortBySize);
}

bool Memory::deleteFromMemory(PCB &pcb) {
	if (pcb.getMemoryPos() == -1)
		return false;
	FST.push_back(pair<int, int>(pcb.getMemoryPos(), pcb.getJobSize()));
	mergeAdjacentSpaces();
	pcb.setMemoryPos(-1);
	pop();
	printFST();
	return true;
}

int Memory::findSpot(int jobSize) {
	for (int i = 0; i < FST.size(); i++)
		if (FST[i].second >= jobSize) {
			return i;
		}
	return -1;
}

int Memory::findMemPos(PCB p) {
	int i = findSpot(p.getJobSize());
	if (i != -1)
		return FST[i].first;
	return i;
}

void Memory::printFST() {
	for (pair<int, int> p : FST) {
		cout << p.first << " " << p.second << endl;
	}
}

PCB & Memory::getNextJob() {
	if (jobs.empty())
		NULL;
	return jobs.front();
}

bool Memory::isEmpty() {
	return jobs.empty();
}

void Memory::pop() {
	jobs.pop();
}

void Memory::push(PCB p) {
	jobs.push(p);
}

int Memory::getCount() {
	return jobs.size();
}

