#pragma once

#include "Definitions.h"
#include "SignalGrid.h"
#include "Connection.h"
#include "Segment.h"
#include <vector>
#include "Correlogram.h"

class LEGION
{
private:
	void initializeGrid();
	void createConnections(std::string name);
	void findLeaders();
	double accuracyRequirement(int channel);
	Oscillator* maxExcitement();
public:
	int CHANNELS;
	int FRAMES;
	int minGroup = 10;
	int leaderNeighbors = 1;
	int numSegments = 0;
	int largestSegment = -1;
	int numInLargestSegment = -1;
	int highFreqThreshold = 54;
	double crossCorrelationThreshold = .95;
	double crossCorrelationThresholdHigh = .7;
	Correlogram& correlogram;
	//Connection*** timeConnections;
	//Connection*** freqConnections;
	Oscillator*** neuralGrid;
	intGrid* segmentGrid;
	std::vector<Segment*>*  segments;

	LEGION(Correlogram& correlogram, std::string name);
	void run();
	void allStep(int spiked);
	boolGrid* segmentsAsMask(int segment);
	void saveSegmentGrid(std::string location);
	void markLargestSegment();
	~LEGION();
};

