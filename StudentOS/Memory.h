#ifndef Memory_H
#define Memory_H
#include "PCB.h"
#include <vector>
using namespace std;
class Memory
{
private:
	vector<pair<int,int> > FST;
public:
	Memory();
	bool insertNewJob(PCB);
	int findSpot(int);
	static bool sortFunction(pair<int, int>, pair<int, int>);
};

#endif