#pragma once
#include "Definitions.h"
#include "Correlogram.h"
#include "GlobalInhibitor.h"
#include "Oscillator(old).h"
#include "Connection(old).h"

class LEGIONold
{
private:
	double crossCorrelationThreshold = .95;
	double globalSpikeThreshold = 1.5;
	double spikeThreshold = -.5;
	int pointsPerPhase = 62;
	double stepSize = .1;

	bool isAnOscillatorSpiking();
	
	void calculateNeighborWeights();
	void initializeGrid(SignalGrid& correlogram);
	void createConnections(SignalGrid& correlogram);
	int update();
public:
	int FRAMES;
	int CHANNELS;
	ConnectionOld*** timeConnections;
	ConnectionOld*** freqConnections;
	OscillatorOld*** neuralGrid;
	GlobalInhibitor* globalInhibitor;

	LEGIONold(SignalGrid& correlogram);
	void run(int phases);
	void saveActiveText(std::string flieName);
	~LEGIONold();
};

