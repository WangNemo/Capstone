#pragma once
#include "GroupingOscillator.h"
class GroupingOscillator;

class Segment
{
private:
	int inserted = 0;
public:
	int segmentSize;
	int active = 0;
	RowColumn** segment;

	Segment();
	Segment(int segmentSize);
	void add(RowColumn* pair);
	int numActive(GroupingOscillator*** neuralGrid);
	~Segment();
};

