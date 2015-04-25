#pragma once
#include "Definitions.h"
#include "SignalBank.h"
#include "SignalGrid.h"

class Correlogram
{
public:
	SignalGrid* T_FGrid;
	Correlogram(SignalBank& cochleagram, int frameSize, int frameOffset);
	boolGrid* toBinaryMask();
	~Correlogram();
};

