#include "Memory.h"
#include <algorithm>


Memory::Memory()
{
	FST.push_back(pair<int, int>(0, 100));
}

bool Memory::sortFunction(pair<int, int> left, pair<int, int> right) {
	return left.second < right.second;
}

bool Memory::insertNewJob(PCB newJob) {
	if (int i = findSpot(newJob.getJobSize() != -1)) {
		pair<int, int> oldSpace = FST[i];
		if (oldSpace.second - newJob.getJobSize() != 0) {
			newJob.setMemoryPos(oldSpace.first);
			FST[i].first = oldSpace.first + newJob.getJobSize() + 1;
			FST[i].second = oldSpace.second - newJob.getJobSize();

		}
		else
			FST.erase(FST.begin() + i);
	}
	else
		return false;
	std::sort(FST.begin(), FST.end(), sortFunction);
	return true;
}

int Memory::findSpot(int jobSize) {
	for (int i = 0; i < FST.size(); i++)
		if (FST[i].second >= jobSize)
			return i;
	return -1;
}