#pragma once
#include "GroupingOscillator.h"
class GroupingOscillator;

class Segment
{
public:
	int inserted = 0;
	int segmentSize;
	int active = 0;
	RowColumn** segment;

	Segment();
	Segment(int segmentSize);
	void add(RowColumn* pair);
	int numActive(GroupingOscillator*** neuralGrid);
	void deleteSeg();
	~Segment();
};

