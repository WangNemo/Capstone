#pragma once
#include "Definitions.h"
#include "LEGION.h"
#include "Oscillator.h"
#include "GroupingOscillator.h"

class GroupingNetwork
{
public:
	GroupingOscillator*** neuralGrid;

	LEGION& legion;
	GroupingNetwork(LEGION& legion );
	~GroupingNetwork();
};

