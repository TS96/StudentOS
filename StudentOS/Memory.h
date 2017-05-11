#ifndef Memory_H
#define Memory_H
#include "PCB.h"
#include <vector>
#include <queue>
using namespace std;
class Memory
{
private:
	vector<pair<int,int> > FST;		//Free space table
	int findSpot(PCB *);			//helper method that finds a free index in the FST
	vector<PCB*> jobs;				//STS list of jobs
	PCB* jobDoingIO;				//PCB that holds the job currently doing IO
public:
	Memory();
	bool insertNewJob(PCB *);
	void mergeAdjacentSpaces();
	bool deleteFromMemory(PCB *);
	static bool sortBySize(pair<int, int>, pair<int, int>);
	static bool sortByAddress(pair<int, int>, pair<int, int>);
	static bool sortByMaxCPUTime(PCB*, PCB*);
	static bool sortByPCBSize(PCB*, PCB*);
	static bool sortIO(PCB*, PCB*);
	static bool sortByLeastCPUTime(PCB*, PCB*);
	static bool sortByBiggestSize(PCB*, PCB*);
	static bool sortByRemainingTime(PCB*, PCB*);
	static bool sortByBiggestRemainingTime(PCB*, PCB*);
	void printFST();
	PCB* getNextJob();
	bool isEmpty();
	void pop();
	void push(PCB*);
	int getCount();
	int findMemPos(PCB*);
	void blockJob();
	void killAfterIO(PCB*);
	void setJobDoingIO(PCB*);
	PCB* getJobDoingIO();
	PCB* findLargestJob();
};

#endif