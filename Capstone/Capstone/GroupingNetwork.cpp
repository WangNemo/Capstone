#include "stdafx.h"
#include "GroupingNetwork.h"
#include "IdealBinaryMask.h"


GroupingNetwork::GroupingNetwork(LEGION& legion)
	: legion(legion), CHANNELS(legion.CHANNELS), FRAMES(legion.FRAMES)
{
	groups = new std::vector<Group*>();
	//neuralGrid = new GroupingOscillator**[CHANNELS];

	//int alive = 0;
	//int match = 0;
	//deadOscillator = new GroupingOscillator();
	for (int channel = 0; channel < CHANNELS; channel++) {
		//neuralGrid[channel] = new GroupingOscillator*[FRAMES];
		for (int frame = 0; frame < FRAMES; frame++) {
			int segNum = legion.neuralGrid[channel][frame]->segment;
			if (segNum >= 0) {
				//alive++;
				//neuralGrid[channel][frame] = new GroupingOscillator(segNum >= 0 ? .2 + (/*legion.neuralGrid[channel][frame]->largestSegment ? .01 :*/ 0) : -100, *(*legion.segments)[segNum]);
				/*if (legion.neuralGrid[channel][frame]->largestSegment) {
					neuralGrid[channel][frame]->largest = true;
				}*/
				int time = legion.correlogram.fundamentialFrequencyTime[frame];
				if ((*legion.correlogram.T_FGrid)[frame][channel][time] / (*legion.correlogram.T_FGrid)[frame][channel][0] > freqMatchThreshold)  {
					//match++;
					(*legion.segments)[segNum]->incrementAgree();
					//neuralGrid[channel][frame]->fundamentalFreqencyMatch = true;
				}
			}
			else {
				//neuralGrid[channel][frame] = deadOscillator;
			}
		}
	}

	int largestSize = 0;
	for (int i = 0; i < legion.numSegments; i++) {
		Segment* seg = (*legion.segments)[i];
		seg->decide();
		if (seg->segmentSize > largestSize) {
			largestSize = seg->segmentSize;
			nextLargestSegment = seg;
		}
	}

	//print alive endl;
	//print match endl;

	//for (int i = 0; i < legion.numSegments; i++) {
	//	Segment seg = *(*legion.segments)[i];
	//	int agree = 0;
	//	int disagree = 0;
	//	for (int pair = 0; pair < seg.segmentSize; pair++) {
	//		if (neuralGrid[seg.segment[pair]->row][seg.segment[pair]->col]->fundamentalFreqencyMatch) {
	//			agree++;
	//		}
	//		else {
	//			disagree++;
	//		}
	//	}
	//	if (agree != 0 && disagree != 0) {
	//		bool majority = agree > disagree;
	//		seg.fundamentalFreqencyMatch = majority;
	//		//for (int pair = 0; pair < seg.segmentSize; pair++) {
	//		//	neuralGrid[seg.segment[pair]->row][seg.segment[pair]->col]->fundamentalFreqencyMatch = majority;
	//		//}
	//	}
	//}
}


void GroupingNetwork::run() {

	//while (nextLargestSegment != nullptr) {
		group();
	groups->push_back(new Group());
		numGroups++;
	//};

	background = (*groups)[0]->backgroundMask(CHANNELS, FRAMES);
	foreground = (*groups)[0]->foregroundMask(CHANNELS, FRAMES);

	//int agreesFiring = 0;
	//int disagreesFiring = 0;
	//int minFrame = 1000, maxFrame = -1;
	//for (int frame = 0; frame < FRAMES; frame++) {
	//	for (int channel = 0; channel < CHANNELS; channel++) {
	//		if (neuralGrid[channel][frame]->largest) {
	//			if (frame < minFrame) minFrame = frame;
	//			if (frame > maxFrame) maxFrame = frame;
	//			neuralGrid[channel][frame]->excitement = SPIKING;
	//			if (neuralGrid[channel][frame]->fundamentalFreqencyMatch) {
	//				agreesFiring++;
	//			}
	//			else {
	//				disagreesFiring++;
	//			}
	//		}
	//	}
	//}
	//bool largestAgrees = agreesFiring < disagreesFiring;

	//int more = 0;
	//for (int frame = minFrame; frame <= maxFrame; frame++) {
	//	for (int channel = 0; channel < CHANNELS; channel++) {
	//		if (neuralGrid[channel][frame]->inputValue >= 0 && neuralGrid[channel][frame]->fundamentalFreqencyMatch
	//			== largestAgrees) {
	//			more++;
	//			neuralGrid[channel][frame]->excitement = SPIKING;
	//		}
	//	}
	//}

	//for (int i = 0; i < legion.numSegments; i++) {
	//	Segment seg = *(*legion.segments)[i];
	//	int spiking = 0;
	//	int notSpiking = 0;
	//	for (int pair = 0; pair < seg.segmentSize; pair++) {
	//		if (neuralGrid[seg.segment[pair]->row][seg.segment[pair]->col]->excitement >= SPIKE_THRESHOLD) {
	//			spiking++;
	//		}
	//		else {
	//			notSpiking++;
	//		}
	//	}
	//	double ratio = abs((float)spiking) / abs(notSpiking);
	//	double exit = (notSpiking == 0 ? true : (ratio > .50 ? SPIKING : MINIMUM_ACTIVITY));
	//	for (int pair = 0; pair < seg.segmentSize; pair++) {
	//		neuralGrid[seg.segment[pair]->row][seg.segment[pair]->col]->excitement = exit;
	//	}
	//}

	////saveActiveText("phase2Active2.txt");
	//foreground = getMask();


	//for (int frame = minFrame; frame <= maxFrame; frame++) {
	//	for (int channel = 0; channel < CHANNELS; channel++) {
	//		if (neuralGrid[channel][frame]->excitement > SPIKE_THRESHOLD) {
	//			neuralGrid[channel][frame]->excitement = MINIMUM_ACTIVITY;
	//		}
	//		else {
	//			if (neuralGrid[channel][frame]->inputValue >= 0) {
	//				neuralGrid[channel][frame]->excitement = SPIKING;
	//			}
	//		}
	//	}
	//}

	//background = getMask();

}

void GroupingNetwork::group() {
	largestSegment = nextLargestSegment;
	largestSegment->grouped = true;

	nextLargestSegment = nullptr;

	Group* group = new Group();
	group->addForeground(largestSegment);

	int largestSize = 0;
	for (int i = 0; i < legion.numSegments; i++) {
		Segment* seg = (*legion.segments)[i];
		if (!seg->grouped) {
			if (seg->segmentSize > largestSize) {
				largestSize = seg->segmentSize;
				nextLargestSegment = seg;
			}
			if (largestSegment->overlapsWith(*seg)) {
				seg->grouped = true;
				if (largestSegment->fundamentalFreqencyMatch == seg->fundamentalFreqencyMatch) {
					group->addForeground(seg);
				}
				else {
					group->addBackground(seg);
				}
			}
		}
	}

	groups->push_back(group);
}

void GroupingNetwork::saveActiveText(std::string flieName) {
	//std::fstream os(flieName, std::ios::out);
	//for (int channel = CHANNELS - 1; channel >= 0; channel--) {
	//	for (int frame = 0; frame < FRAMES; frame++) {
	//		os << (neuralGrid[channel][frame]->excitement > SPIKE_THRESHOLD);
	//		os << " ";
	//	}
	//	os << "\n";
	//}
}

boolGrid* GroupingNetwork::getMask() {
	//bool** grid = new bool*[CHANNELS];
	//for (int channel = CHANNELS - 1; channel >= 0; channel--) {
	//	grid[channel] = new bool[FRAMES];
	//	for (int frame = 0; frame < FRAMES; frame++) {
	//		grid[channel][frame] = (neuralGrid[channel][frame]->excitement > SPIKE_THRESHOLD);
	//	}
	//}
	//return new boolGrid(grid, CHANNELS, FRAMES);
	return nullptr;
}


GroupingNetwork::~GroupingNetwork()
{
	//for (int channel = 0; channel < CHANNELS; channel++) {
	//	for (int frame = 0; frame < FRAMES; frame++) {
	//		if (neuralGrid[channel][frame] != deadOscillator)
	//			delete neuralGrid[channel][frame];
	//	}
	//	delete[] neuralGrid[channel];
	//}
	//delete deadOscillator;
	//delete[] neuralGrid;
	for (int i = 0; i < numGroups; i++) {
		(*groups)[i]->deleteGroup();
		delete ((*groups)[i]);
	}
	groups->clear();
	delete groups;

	delete foreground;
	delete background;
}
