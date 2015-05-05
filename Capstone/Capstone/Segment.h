#pragma once
#include "GroupingOscillator.h"

class Segment
{
private:
	int inserted = 0;
public:
	int segmentSize;
	RowColumn** segment;

	Segment(int segmentSize);
	void add(RowColumn* pair);
	int numActive(GroupingOscillator*** neuralGrid);
	~Segment();
};

