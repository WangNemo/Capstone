#pragma once
#include "Definitions.h"
#include "SignalBank.h"
#include "SignalGrid.h"

class Correlogram
{
public:
	double activityThreshold = .6;

	SignalGrid* T_FGrid;
	int* fundamentialFrequencyTime;
	Correlogram(SignalBank& cochleagram, int frameSize, int frameOffset);
	boolGrid* toBinaryMask();
	~Correlogram();
};

