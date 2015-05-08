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
	void createConnections();
	void findLeaders();
	Oscillator* maxExcitement();
public:
	int CHANNELS;
	int FRAMES;
	int minGroup = 3;
	int leaderNeighbors = 1;
	int numSegments = 0;
	int largestSegment = -1;
	int numInLargestSegment = -1;
	double crossCorrelationThreshold = .95;
	Correlogram& correlogram;
	//Connection*** timeConnections;
	//Connection*** freqConnections;
	Oscillator*** neuralGrid;
	intGrid* segmentGrid;
	std::vector<Segment*>*  segments;

	LEGION(Correlogram& correlogram);
	void run();
	void allStep(int spiked);
	boolGrid* segmentsAsMask();
	void saveSegmentGrid(std::string location);
	void markLargestSegment();
	~LEGION();
};

