#pragma once
#include "Definitions.h"
#include "SignalGrid.h"
#include "Signal.h"

class crossCorrelationSegmentation
{
private:
	void traverse(int row, int col);
public:
	const int UNASSIGNED = -1;

	//SignalGrid& correlogram;
	boolGrid& booleanGrid;
	intGrid* segmentGrid;
	int groups = 0;

	crossCorrelationSegmentation(boolGrid& grid);
	boolGrid* getBinaryMask(int group);
	int getLargestSegment();
	int segmentSize(int group);
	void writeSegmentText(std::string name);
	~crossCorrelationSegmentation();
};

