/*
Tarek Saidee
CISC 3320
*/

#include "Swapper.h"
#include "Memory.h"

void siodrum(int jobnum, int jobsize, int coreaddress, int direction);

//constructor
Swapper::Swapper()		
{
	swappingIn = false;
	swappingOut = false;
}

//check if a job is being swapped in
bool Swapper::isSwappingIn() {		
	return swappingIn;
}

//check if a job is being swapped out
bool Swapper::isSwappingOut() {		
	return swappingOut;
}

//change swapping in status
void Swapper::setSwappingIn(bool status) {
	swappingIn = status;
}

//change swapping out status
void Swapper::setSwappingOut(bool status) {
	swappingOut = status;
}

//return the job currently being swapped
PCB* Swapper::getJobBeingSwapped() {
	return beingSwapped;
}

//push a job onto the LTS
void Swapper::addToLTS(PCB* newJob) {
	LTS.push(newJob);
}

//run a job from the LTS
void Swapper::runFromLTS(int &a, int p[], Memory& memory) {
	cout << "Run from LTS " << LTS.size() << endl;
	swapFromLTS(a, p, memory);
	a = 1;
}

//swap in a job if the drum isn't busy
bool Swapper::swapIn(int &a, int p[], PCB *temp, int memoryPos) {
	if (!swappingIn && !swappingOut) {
		siodrum(temp->getJobNumber(), temp->getJobSize(), memoryPos, 0);
		swappingIn = true;
		beingSwapped = temp;
		return true;
	}
	return false;
}

/*
	swap out a job if the drum isn't busy. Also check if the job is not doing IO or has no pending IO or isn't in memory
	because the status of the job might have changed after being pushed onto the swap out queue such as finishing IO
	or terminating. If a job was marked to be swapped out and isn't in memory anymore then that means
	it terminated and should be deleted. If a job is too big then it should be put on the LTS
	to be executed later on.
*/
void Swapper::swapOut(int &a, int p[], Memory& memory) {
	if (!swappingIn && !swappingOut && !swapOutQ.empty()) {
		if ((swapOutQ.back()->isDoingIO() || swapOutQ.back()->getPendingIO() == 0 || !swapOutQ.back()->isInMemory()) && !swapOutQ.back()->isTooBig()) {
			if (swapOutQ.back()->toSwapOut() && !swapOutQ.back()->isInMemory())
				delete swapOutQ.back();
			swapOutQ.pop_back();
			return;
		}
		swapOutQ.back()->setBlocked(true);
		siodrum(swapOutQ.back()->getJobNumber(), swapOutQ.back()->getJobSize(), swapOutQ.back()->getMemoryPos(), 1);
		swapOutQ.back()->setToSwapOut(false);
		swappingOut = true;
		memory.deleteFromMemory(swapOutQ.back());
		if (swapOutQ.back()->isTooBig()) {
			swapOutQ.back()->setTooBig(false);
			swapOutQ.back()->setBlocked(false);
			LTS.push(swapOutQ.back());
		}
		swapOutQ.pop_back();
	}
}

/*
Swap in a new job from the LTS, if there is one avaiable and memory allows it.
This is done to keep the disk busy.
*/
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

//Push a job onto the swap out vector and sorts it
void Swapper::addToSwapOutQ(PCB* job) {
	job->setToSwapOut(true);
	swapOutQ.push_back(job);
	std::sort(swapOutQ.begin(), swapOutQ.end(), Memory::sortByBiggestRemainingTime);
}

//Check if the swap out vector is empty
bool Swapper::swapOutQEmpty() {
	return swapOutQ.empty();
}