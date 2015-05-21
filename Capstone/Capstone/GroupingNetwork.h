#pragma once
#include "Definitions.h"
#include "LEGION.h"
#include "Oscillator.h"
#include "Group.h"
//#include "GroupingOscillator.h"

class GroupingNetwork
{
private:
	//GroupingOscillator* deadOscillator;
	void setFreqTreshold();
	double getAgreementPercent(double matchThreshold);
public:
	int CHANNELS, FRAMES;
	int numGroups = 0;
	//GroupingOscillator*** neuralGrid;
	boolGrid* foreground;
	boolGrid* background;
	Segment* largestSegment;
	Segment* nextLargestSegment;
	std::vector<Group*>* groups;
	double freqMatchThreshold = .5;

	LEGION& legion;
	GroupingNetwork(LEGION& legion);
	void run();
	void group();
	void saveActiveText(std::string flieName);
	boolGrid* getMask();
	~GroupingNetwork();
};

