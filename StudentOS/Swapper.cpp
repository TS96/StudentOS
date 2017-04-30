#include "Swapper.h"
#include "Memory.h"

void siodrum(int jobnum, int jobsize, int coreaddress, int direction);


Swapper::Swapper()
{
	swappingIn = false;
	swappingOut = false;
}

bool Swapper::isSwappingIn() {
	return swappingIn;
}

bool Swapper::isSwappingOut() {
	return swappingOut;
}

void Swapper::setSwappingIn(bool status) {
	swappingIn = status;
}

void Swapper::setSwappingOut(bool status) {
	swappingOut = status;
}

PCB* Swapper::getJobBeingSwapped() {
	return beingSwapped;
}

void Swapper::addToLTS(PCB* newJob) {
	LTS.push_back(newJob);
	std::sort(LTS.begin(), LTS.end(), Memory::sortByRemainingTime);
}

void Swapper::runFromLTS(int &a, int p[], Memory& memory) {
	cout << "Run from LTS " << LTS.size() << endl;
	swapFromLTS(a, p, memory);
	a = 1;
}

bool Swapper::swapIn(int &a, int p[], PCB *temp, int memoryPos) {
	if (!swappingIn && !swappingOut) {
		siodrum(temp->getJobNumber(), temp->getJobSize(), memoryPos, 0);
		swappingIn = true;
		beingSwapped = temp;
		return true;
	}
	return false;
}

void Swapper::swapOut(int &a, int p[], Memory& memory) {
	if (!swappingIn && !swappingOut && !swapOutQ.empty()) {
		if ((swapOutQ.front()->isDoingIO() || swapOutQ.front()->getPendingIO() == 0 || !swapOutQ.front()->isInMemory()) && !swapOutQ.front()->isTooBig()) {
			swapOutQ.pop();
			return;
		}
		swapOutQ.front()->setBlocked(true);
		siodrum(swapOutQ.front()->getJobNumber(), swapOutQ.front()->getJobSize(), swapOutQ.front()->getMemoryPos(), 1);
		swappingOut = true;
		memory.deleteFromMemory(swapOutQ.front());
		if (swapOutQ.front()->isTooBig()) {
			swapOutQ.front()->setTooBig(false);
			swapOutQ.front()->setBlocked(false);
			LTS.push_back(swapOutQ.front());
			std::sort(LTS.begin(), LTS.end(), memory.sortByRemainingTime);
		}
		swapOutQ.pop();
	}
}

void Swapper::swapFromLTS(int &a, int p[], Memory& memory) {
	if (!LTS.empty()) {
		PCB* temp = LTS.back();
		int memoryPos = memory.findMemPos(temp);
		if (memoryPos != -1) {
			if (swapIn(a, p, temp, memoryPos))
				LTS.pop_back();
		}
	}
}