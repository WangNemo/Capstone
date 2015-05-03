#pragma once
#include "Definitions.h"
#include "Correlogram.h"
#include "GlobalInhibitor.h"
#include "Oscillator.h"

class LEGION
{
private:
	double crossCorrelationThreshold = .70;
	double spikeThreshold = 1.5;
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
	Connection*** timeConnections;
	Connection*** freqConnections;
	Oscillator*** neuralGrid;
	GlobalInhibitor* globalInhibitor;

	LEGION(SignalGrid& correlogram);
	void run(int phases);
	void saveActiveText(std::string flieName);
	~LEGION();
};

