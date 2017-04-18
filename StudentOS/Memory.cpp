#include "Memory.h"
#include <algorithm>
#include <iostream>


Memory::Memory()
{
	FST.push_back(pair<int, int>(0, 100));
	beKilled = false;
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
	//printFST();
	for (int i = 0; i < FST.size() - 1; i++) {
		if (FST[i].first + FST[i].second + ((FST[i].first%10==0)?0:1) == FST[i + 1].first) {
			//cout << FST[i].second << endl;
			FST[i].second += FST[i + 1].second;
			//cout << FST[i].second << endl;
			FST.erase(FST.begin() + i + 1);
			mergeAdjacentSpaces();
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
	if (!shouldKill())
		pop();
	else
		beKilled = false;
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

void Memory::blockJob() {
	jobs.front().setBlocked(true);
	jobDoingIO = jobs.front();
	jobs.pop();
}

void Memory::killAfterIO(PCB p) {
	cout << "THIS HAPPENED!!!!!!!!!!!!!!!!!!!" << endl;
	jobDoingIO = p;
	beKilled = true;
	pop();
}

void Memory::setJobDoingIO(PCB p) {
	jobDoingIO = p;
}

PCB& Memory::getJobDoingIO() {
	return jobDoingIO;
}

bool Memory::shouldKill() {
	return beKilled;
}