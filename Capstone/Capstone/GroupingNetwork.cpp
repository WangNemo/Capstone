#include "stdafx.h"
#include "GroupingNetwork.h"
#include "IdealBinaryMask.h"


GroupingNetwork::GroupingNetwork(LEGION& legion)
	: legion(legion), CHANNELS(legion.CHANNELS), FRAMES(legion.FRAMES)
{
	neuralGrid = new GroupingOscillator**[CHANNELS];

	int alive = 0;
	int match = 0;
	deadOscillator = new GroupingOscillator();
	for (int channel = 0; channel < CHANNELS; channel++) {
		neuralGrid[channel] = new GroupingOscillator*[FRAMES];
		for (int frame = 0; frame < FRAMES; frame++) {
			int segNum = legion.neuralGrid[channel][frame]->segment;
			if (segNum >= 0) {
				alive++;
				neuralGrid[channel][frame] = new GroupingOscillator(segNum >= 0 ? .2 + (legion.neuralGrid[channel][frame]->largestSegment ? .01 : 0) : -100, *(*legion.segments)[segNum]);
				if (legion.neuralGrid[channel][frame]->largestSegment) {
					neuralGrid[channel][frame]->largest = true;
				}
				int time = legion.correlogram.fundamentialFrequencyTime[frame];
				if ((*legion.correlogram.T_FGrid)[frame][channel][time] / (*legion.correlogram.T_FGrid)[frame][channel][0] > freqMatchThreshold)  {
					match++;
					neuralGrid[channel][frame]->fundamentalFreqencyMatch = true;
				}
			}
			else {
				neuralGrid[channel][frame] = deadOscillator;
			}
		}
	}

	print alive endl;
	print match endl;

	for (int i = 0; i < legion.numSegments; i++) {
		Segment seg = *(*legion.segments)[i];
		int agree = 0;
		int disagree = 0;
		for (int pair = 0; pair < seg.segmentSize; pair++) {
			if (neuralGrid[seg.segment[pair]->row][seg.segment[pair]->col]->fundamentalFreqencyMatch) {
				agree++;
			}
			else {
				disagree++;
			}
		}
		if (agree != 0 && disagree != 0) {
			bool majority = agree > disagree;
			for (int pair = 0; pair < seg.segmentSize; pair++) {
				neuralGrid[seg.segment[pair]->row][seg.segment[pair]->col]->fundamentalFreqencyMatch = majority;
			}
		}
	}
}


void GroupingNetwork::run() {
	int agreesFiring = 0;
	int disagreesFiring = 0;
	int minFrame = 1000, maxFrame = -1;
	for (int frame = 0; frame < FRAMES; frame++) {
		for (int channel = 0; channel < CHANNELS; channel++) {
			if (neuralGrid[channel][frame]->largest) {
				if (frame < minFrame) minFrame = frame;
				if (frame > maxFrame) maxFrame = frame;
				neuralGrid[channel][frame]->excitement = SPIKING;
				if (neuralGrid[channel][frame]->fundamentalFreqencyMatch) {
					agreesFiring++;
				}
				else {
					disagreesFiring++;
				}
			}
		}
	}
	bool largestAgrees = agreesFiring < disagreesFiring;

	int more = 0;
	for (int frame = minFrame; frame <= maxFrame; frame++) {
		for (int channel = 0; channel < CHANNELS; channel++) {
			if (neuralGrid[channel][frame]->inputValue >= 0 && neuralGrid[channel][frame]->fundamentalFreqencyMatch
				== largestAgrees) {
				more++;
				neuralGrid[channel][frame]->excitement = SPIKING;
			}
		}
	}

	for (int i = 0; i < legion.numSegments; i++) {
		Segment seg = *(*legion.segments)[i];
		int spiking = 0;
		int notSpiking = 0;
		for (int pair = 0; pair < seg.segmentSize; pair++) {
			if (neuralGrid[seg.segment[pair]->row][seg.segment[pair]->col]->excitement >= SPIKE_THRESHOLD) {
				spiking++;
			}
			else {
				notSpiking++;
			}
		}
		double ratio = abs((float)spiking) / abs(notSpiking);
		double exit = (notSpiking == 0 ? true : (ratio > .50 ? SPIKING : MINIMUM_ACTIVITY));
		for (int pair = 0; pair < seg.segmentSize; pair++) {
			neuralGrid[seg.segment[pair]->row][seg.segment[pair]->col]->excitement = exit;
		}
	}

	//saveActiveText("phase2Active2.txt");
	foreground = getMask();


	for (int frame = minFrame; frame <= maxFrame; frame++) {
		for (int channel = 0; channel < CHANNELS; channel++) {
			if (neuralGrid[channel][frame]->excitement > SPIKE_THRESHOLD) {
				neuralGrid[channel][frame]->excitement = MINIMUM_ACTIVITY;
			}
			else {
				if (neuralGrid[channel][frame]->inputValue >= 0) {
					neuralGrid[channel][frame]->excitement = SPIKING;
				}
			}
		}
	}

	background = getMask();

	//print agreesFiring endl;
	//print disagreesFiring endl;
	//print more endl;

	//saveActiveText("phase2Active1.txt");


	//for (int i = 0; i < 10000; i++) {
	//	for (int i = 0; i < legion.numSegments; i++) {
	//		(*legion.segments)[i].numActive(neuralGrid);
	//	}

	//	int active = 0;

	//	for (int frame = 0; frame < FRAMES; frame++) {
	//		int agreesFiring = 0;
	//		int disagreesFiring = 0;
	//		for (int channel = 0; channel < CHANNELS; channel++) {
	//			if (neuralGrid[channel][frame]->excitement > SPIKE_THRESHOLD) {
	//				if (neuralGrid[channel][frame]->fundamentalFreqencyMatch) {
	//					agreesFiring++;
	//				}
	//				else {
	//					disagreesFiring++;
	//				}
	//			}
	//		}
	//		double neighborWeight = agreesFiring * MUTUAL_EXCITEMENT + disagreesFiring * MUTUAL_INHIBITION;
	//		for (int channel = 0; channel < CHANNELS; channel++) {
	//			neuralGrid[channel][frame]->neighborWeights = neuralGrid[channel][frame]->fundamentalFreqencyMatch ? neighborWeight : -neighborWeight;
	//			neuralGrid[channel][frame]->update();
	//			if (neuralGrid[channel][frame]->excitement > SPIKE_THRESHOLD)
	//				active++;
	//		}
	//	}
	//	print i endl;
	//	if (/*i > 1000 &&*/ active > 250) {
	//		print '\t' << "active: " << active endl;
	//		saveActiveText("phase2Active.txt");
	//	}
	//}
}

void GroupingNetwork::saveActiveText(std::string flieName) {
	std::fstream os(flieName, std::ios::out);
	for (int channel = CHANNELS - 1; channel >= 0; channel--) {
		for (int frame = 0; frame < FRAMES; frame++) {
			os << (neuralGrid[channel][frame]->excitement > SPIKE_THRESHOLD);
			os << " ";
		}
		os << "\n";
	}
}

boolGrid* GroupingNetwork::getMask() {
	bool** grid = new bool*[CHANNELS];
	for (int channel = CHANNELS - 1; channel >= 0; channel--) {
		grid[channel] = new bool[FRAMES];
		for (int frame = 0; frame < FRAMES; frame++) {
			grid[channel][frame] = (neuralGrid[channel][frame]->excitement > SPIKE_THRESHOLD);
		}
	}
	return new boolGrid(grid, CHANNELS, FRAMES);
}


GroupingNetwork::~GroupingNetwork()
{
	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {
			if (neuralGrid[channel][frame] != deadOscillator)
				delete neuralGrid[channel][frame];
		}
		delete[] neuralGrid[channel];
	}
	delete deadOscillator;
	delete[] neuralGrid;

	delete foreground;
	delete background;
}
