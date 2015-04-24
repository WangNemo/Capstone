#pragma once
#include "Definitions.h"
#include "SignalGrid.h"

class crossCorrelationSegmentation
{
public:
	SignalGrid& correlogram;
	intGrid* segmentGrid;

	crossCorrelationSegmentation(SignalGrid& correlogram);
	~crossCorrelationSegmentation();
};

