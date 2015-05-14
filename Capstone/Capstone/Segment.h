#pragma once
#include "GroupingOscillator.h"
class GroupingOscillator;

class Segment
{
public:
	int inserted = 0;
	int segmentSize;
	int active = 0;
	int minFrame = 10000;
	int maxFrame = -1;
	int agree = 0;
	int disagree = 0;
	bool fundamentalFreqencyMatch = false;
	bool grouped = false;
	RowColumn** segment;

	Segment();
	Segment(int segmentSize);
	void add(RowColumn* pair);
	void incrementAgree();
	void incrementDisagree();
	void decide();
	bool overlapsWith(Segment& other);
	//int numActive(GroupingOscillator*** neuralGrid);
	void deleteSeg();
	~Segment();
};

