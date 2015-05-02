#include "stdafx.h"
#include "LEGION.h"


LEGION::LEGION(SignalGrid& correlogram)
{
	/*Oscillator* oss = new Oscillator(.5, .1, .02, .8);
	GlobalInhibitor* gi = new GlobalInhibitor();
	Signal sig(44100, 44100);
	for (int i = 0; i < 44100; i++) {
		sig[i] = gi->inhibition;
		gi->update(oss->excitement > 1.5);
		oss->update();
	}
	sig.save(std::string("gi.wav"));*/

	initializeGrid(correlogram);
	linkNeighbors(correlogram);
}

void LEGION::initializeGrid(SignalGrid& correlogram) {
	print "initializing" end;
	neuralGrid = new Oscillator**[correlogram.CHANNELS];
	for (int channel = 0; channel < correlogram.CHANNELS; channel++) {
		neuralGrid[channel] = new Oscillator*[correlogram.FRAMES];
		for (int frame = 0; frame < correlogram.FRAMES; frame++) {
			int neighbors = 4;
			if (channel == 0 || channel == correlogram.CHANNELS - 1) {
				neighbors--;
			}
			if (frame == 0 || frame == correlogram.FRAMES - 1) {
				neighbors--;
			}
			neuralGrid[channel][frame] = new Oscillator(.5, .1, .02, correlogram[frame][channel][0] > 0 ? .2 : -5);
			(*neuralGrid[channel][frame]).numNeighbors = neighbors;
		}
	}
}

void LEGION::linkNeighbors(SignalGrid& correlogram) {
	print "linking" end;
	Connection*** timeConnections = new Connection**[correlogram.CHANNELS];
	Connection*** freqConnections = new Connection**[correlogram.CHANNELS - 1];
	double crosses = 0;

	for (int channel = 0; channel < correlogram.CHANNELS; channel++) {
		timeConnections[channel] = new Connection*[correlogram.FRAMES - 1];
		if (channel < correlogram.CHANNELS - 1)
			freqConnections[channel] = new Connection*[correlogram.FRAMES];

		for (int frame = 0; frame < correlogram.FRAMES; frame++) {
			if (frame < correlogram.FRAMES - 1) {
				Connection* timeConnection = new Connection(1, neuralGrid[channel][frame], neuralGrid[channel][frame + 1]);
				timeConnections[channel][frame] = timeConnection;
			}
			if (channel < correlogram.CHANNELS - 1) {
				double weight = correlogram[frame][channel].crossCorrelate(correlogram[frame][channel + 1]) 
					> crossCorrelationThreshold ? 1 : 0;
				if (weight != 0)
					print crosses end;

				crosses += weight;
				Connection* freqConnection = new Connection(weight, neuralGrid[channel][frame], neuralGrid[channel + 1][frame]);
				freqConnections[channel][frame] = freqConnection;
			}
		}
	}
	print crosses end;
}


LEGION::~LEGION()
{
}
