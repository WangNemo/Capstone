#pragma once
#include "Definitions.h"
#include "SignalGrid.h"
#include "Signal.h"

class crossCorrelationSegmentation
{
private:
	void traverse(int row, int col);
	double crossCorrelation(Signal& one, Signal& two);
public:
	const int UNASSIGNED = -1;

	SignalGrid& correlogram;
	intGrid* segmentGrid;
	int groups = 1;

	crossCorrelationSegmentation(SignalGrid& correlogram);
	boolGrid* getBinaryMask(int group);
	~crossCorrelationSegmentation();
};

