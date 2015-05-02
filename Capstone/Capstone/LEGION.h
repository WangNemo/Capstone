#pragma once
#include "Correlogram.h"
#include "GlobalInhibitor.h"
#include "Oscillator.h"

class LEGION
{
private:
	void initializeGrid(SignalGrid& correlogram);
	void linkNeighbors(SignalGrid& correlogram);
	double crossCorrelationThreshold = .985;
public:
	Oscillator*** neuralGrid;

	LEGION(SignalGrid& correlogram);
	~LEGION();
};

