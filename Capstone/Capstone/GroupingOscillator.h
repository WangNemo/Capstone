#pragma once
#include "Definitions.h"
#include "Segment.h"

class Segment;

class GroupingOscillator
{
public:
	Segment& segment;
	//double excitement = MINIMUM_ACTIVITY + .5;
	//double inhibition = .5;
	//double inputValue;
	//double neighborWeights;
	bool fundamentalFreqencyMatch = false;
	//bool largest = false;

	GroupingOscillator();
	GroupingOscillator(Segment& segment);
	//GroupingOscillator(double inputValue, Segment& segment);
	//void update();
	~GroupingOscillator();
};

