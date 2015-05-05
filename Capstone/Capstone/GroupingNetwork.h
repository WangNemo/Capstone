#pragma once
#include "Definitions.h"
#include "LEGION.h"
#include "Oscillator.h"
#include "GroupingOscillator.h"

class GroupingNetwork
{
public:
	int CHANNELS, FRAMES;
	GroupingOscillator*** neuralGrid;
	boolGrid* foreground;
	boolGrid* background;

	LEGION& legion;
	GroupingNetwork(LEGION& legion);
	void run();
	void saveActiveText(std::string flieName);
	boolGrid* getMask();
	~GroupingNetwork();
};

