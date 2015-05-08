#pragma once
#include "Definitions.h"
#include "LEGION.h"
#include "Oscillator.h"
#include "GroupingOscillator.h"

class GroupingNetwork
{
private:
	GroupingOscillator* deadOscillator;
public:
	int CHANNELS, FRAMES;
	GroupingOscillator*** neuralGrid;
	boolGrid* foreground;
	boolGrid* background;
	double freqMatchThreshold = .95;

	LEGION& legion;
	GroupingNetwork(LEGION& legion);
	void run();
	void saveActiveText(std::string flieName);
	boolGrid* getMask();
	~GroupingNetwork();
};

