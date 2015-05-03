#include "stdafx.h"
#include "LEGION.h"
#include<fstream>

LEGION::LEGION(SignalGrid& correlogram) : FRAMES(correlogram.FRAMES), CHANNELS(correlogram.CHANNELS)
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
	globalInhibitor = new GlobalInhibitor();
	initializeGrid(correlogram);
	createConnections(correlogram);
}

void LEGION::initializeGrid(SignalGrid& correlogram){
	print "initializing" end;
	double powerThreshold = .75;

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

			double randX = rand() / (RAND_MAX / .8);
			double randY = rand() / (RAND_MAX / .8);
			double noise = staticTools::nonZeroGaussianRandom(.02);
			neuralGrid[channel][frame] = new Oscillator(randX, randY, noise, 
				correlogram[frame][channel][0] > powerThreshold ? .2 : -5,
				neighbors);
		}
	}
}

void LEGION::createConnections(SignalGrid& correlogram) {
	print "linking" end;
	timeConnections = new Connection**[correlogram.CHANNELS];
	freqConnections = new Connection**[correlogram.CHANNELS - 1];
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

				crosses += weight;
				Connection* freqConnection = new Connection(weight, neuralGrid[channel][frame], neuralGrid[channel + 1][frame]);
				freqConnections[channel][frame] = freqConnection;
			}
		}
	}
}

void LEGION::calculateNeighborWeights(){
	double globalInhibition = isAnOscillatorSpiking() ? .5 : 0;
		//globalInhibitor->inhibition > .5 ? .5 : 0;

	int up = 0, down = 1, left = 2, right = 3;
	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {

			double directions[4] {-1, -1, -1, -1};
			double potential = 0;

			if (channel > 0) {
				directions[up] = freqConnections[channel - 1][frame]->getWeightOne();
			}
			if (channel < CHANNELS - 1) {
				directions[down] = freqConnections[channel][frame]->getWeightTwo();
			}
			if (frame > 0) {
				directions[left] = timeConnections[channel][frame - 1]->getWeightOne();
				potential += (timeConnections[channel][frame - 1]->one->excitement > spikeThreshold);
				if (timeConnections[channel][frame - 1]->one->excitement > spikeThreshold) {
					int a = 5;
				}
				else {
					int a = 5;
				}
			}
			if (frame < FRAMES - 1) {
				directions[right] = timeConnections[channel][frame]->getWeightTwo();
				potential += (timeConnections[channel][frame]->two->excitement > spikeThreshold);
				if (timeConnections[channel][frame]->two->excitement > spikeThreshold) {
					int a = 5;
				}
				else {
					int a = 5;
				}
			}

			Oscillator& osc = *neuralGrid[channel][frame];

			double* weights = new double[osc.neighbors];
			int weigh = 0;
			for (int dir = 0; dir < 4; dir++) {
				if (directions[dir] > -1) {
					weights[weigh++] = directions[dir];
				}
			}

			osc.updateNeighborWeights(weights, globalInhibition);
			potential -= spikeThreshold;
			osc.updatePotential(potential, stepSize);

			delete[] weights;
		}
	}
}

bool LEGION::isAnOscillatorSpiking() {
	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {
			if (neuralGrid[channel][frame]->excitement > spikeThreshold)
				return true;
		}
	}
	return false;
}

void LEGION::saveActiveText(std::string flieName) {
	std::fstream os(flieName, std::ios::out);
	for (int channel = CHANNELS - 1; channel >= 0; channel--) {
		for (int frame = 0; frame < FRAMES; frame++) {
			os << (neuralGrid[channel][frame]->excitement > spikeThreshold);
			os << " ";
		}
		os << "\n";
	}
}


void LEGION::run(int phases) {
	print "oscilating " << phases end;
	int steps = phases * (pointsPerPhase * 10);
	Signal* sig = new Signal(steps, 44100);
	int realStep = 0;
	for (int phase = 0; phase < phases; phase++) {
		int maxJump = 0;
		for (int step = 0; step < pointsPerPhase; step++) {
			bool printedSub = false;
			for (int substep = 0; substep < 10; substep++) {
				globalInhibitor->update(stepSize, isAnOscillatorSpiking());
				(*sig)[realStep++] = globalInhibitor->inhibition;

				calculateNeighborWeights();
				int active = update();
				if (active > maxJump) maxJump = active;
				if (active > 1 && !printedSub) {
					printedSub = true;
					print "\t\t" << active end;
					if (phase > 30)
						saveActiveText(std::to_string(phase) + "spikes" + std::to_string(active) + ".txt");
				}
			}
		}
		print "\t\t\tmax" << maxJump end;
		print '\t' << phase end;

	}
	sig->save(std::string("globalOscy.wav"));
}

int LEGION::update() {
	int active = 0;
	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {
			neuralGrid[channel][frame]->update(stepSize);
			if (neuralGrid[channel][frame]->excitement > spikeThreshold)
				active++;
		}
	}
	return active;
}



LEGION::~LEGION()
{
}
