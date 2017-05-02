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
	LTS.push(newJob);
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
			if (swapOutQ.front()->toSwapOut() && !swapOutQ.front()->isInMemory())
				delete swapOutQ.front();
			swapOutQ.pop();
			return;
		}
		swapOutQ.front()->setBlocked(true);
		siodrum(swapOutQ.front()->getJobNumber(), swapOutQ.front()->getJobSize(), swapOutQ.front()->getMemoryPos(), 1);
		swapOutQ.front()->setToSwapOut(false);
		swappingOut = true;
		memory.deleteFromMemory(swapOutQ.front());
		if (swapOutQ.front()->isTooBig()) {
			swapOutQ.front()->setTooBig(false);
			swapOutQ.front()->setBlocked(false);
			LTS.push(swapOutQ.front());
		}
		swapOutQ.pop();
	}
}

void Swapper::swapFromLTS(int &a, int p[], Memory& memory) {
	if (!LTS.empty()) {
		PCB* temp = LTS.top();
		int memoryPos = memory.findMemPos(temp);
		if (memoryPos != -1) {
			if (swapIn(a, p, temp, memoryPos))
				LTS.pop();
		}
	}
}

void Swapper::addToSwapOutQ(PCB* job) {
	job->setToSwapOut(true);
	swapOutQ.push(job);
}

bool Swapper::swapOutQEmpty() {
	return swapOutQ.empty();
}