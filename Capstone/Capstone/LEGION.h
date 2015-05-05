#pragma once

#include "Definitions.h"
#include "SignalGrid.h"
#include "Connection.h"
#include "Segment.h"
#include <vector>

class LEGION
{
private:
	void initializeGrid(SignalGrid& correlogram);
	void createConnections(SignalGrid& correlogram);
	void findLeaders();
	Oscillator* maxExcitement();
public:
	int CHANNELS;
	int FRAMES;
	int minGroup = 3;
	int leaderNeighbors = 2;
	int numSegments = 0;
	int largestSegment = -1;
	int numInLargestSegment = -1;
	double crossCorrelationThreshold = .95;
	Connection*** timeConnections;
	Connection*** freqConnections;
	Oscillator*** neuralGrid;
	intGrid* segmentGrid;
	std::vector<Segment*>*  segments;

	LEGION(SignalGrid& correlogram);
	void run();
	void allStep(int spiked);
	void saveSegmentGrid(std::string location);
	void markLargestSegment();
	~LEGION();
};

