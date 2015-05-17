#include "stdafx.h"
#include "LEGION.h"


LEGION::LEGION(Correlogram& correlogram) : FRAMES(correlogram.T_FGrid->FRAMES), CHANNELS(correlogram.T_FGrid->CHANNELS), correlogram(correlogram)
{
	segments = new std::vector<Segment*>();
	segmentGrid = new intGrid(CHANNELS, FRAMES, -1);
	initializeGrid();
	createConnections();
	findLeaders();
}

void LEGION::initializeGrid() {
	print "initializing" endl;

	neuralGrid = new Oscillator**[CHANNELS];
	for (int channel = 0; channel < CHANNELS; channel++) {
		neuralGrid[channel] = new Oscillator*[FRAMES];
		for (int frame = 0; frame < FRAMES; frame++) {
			double randa = rand();
			double randX = randa / RAND_MAX + MINIMUM_ACTIVITY;
			neuralGrid[channel][frame] = new Oscillator(randX,
				(*correlogram.T_FGrid)[frame][channel][0] > correlogram.activityThreshold ? .2 : -5);
		}
	}
}


void LEGION::createConnections() {
	print "linking" endl;
	//timeConnections = new Connection**[correlogram.CHANNELS];
	//freqConnections = new Connection**[correlogram.CHANNELS - 1];

	for (int channel = 0; channel < CHANNELS; channel++) {
		//timeConnections[channel] = new Connection*[correlogram.FRAMES - 1];
		//if (channel < CHANNELS - 1)
			//freqConnections[channel] = new Connection*[correlogram.FRAMES];

		for (int frame = 0; frame < FRAMES; frame++) {
			if (frame < FRAMES - 1) {
				double weight = (*correlogram.T_FGrid)[frame][channel].crossCorrelate((*correlogram.T_FGrid)[frame + 1][channel])
					> (channel >= 54 ? crossCorrelationThresholdHigh : crossCorrelationThreshold) ? 1 : 0;

				//Connection* timeConnection = new Connection(weight, neuralGrid[channel][frame], neuralGrid[channel][frame + 1]);
				//timeConnections[channel][frame] = timeConnection;
				if (weight > 0) {
					neuralGrid[channel][frame]->right = neuralGrid[channel][frame + 1];
					neuralGrid[channel][frame]->wRight = weight;
					neuralGrid[channel][frame + 1]->left = neuralGrid[channel][frame];
					neuralGrid[channel][frame + 1]->wLeft = weight;
				}
			}
			if (channel < CHANNELS - 1) {
				double weight = (*correlogram.T_FGrid)[frame][channel].crossCorrelate((*correlogram.T_FGrid)[frame][channel + 1])
					> (channel >= 54 ? crossCorrelationThresholdHigh : crossCorrelationThreshold) ? 1 : 0;

				//Connection* freqConnection = new Connection(weight, neuralGrid[channel][frame], neuralGrid[channel + 1][frame]);
				//freqConnections[channel][frame] = freqConnection;
				if (weight > 0) {
					neuralGrid[channel][frame]->down = neuralGrid[channel + 1][frame];
					neuralGrid[channel][frame]->wDown = weight;
					neuralGrid[channel + 1][frame]->up = neuralGrid[channel][frame];
					neuralGrid[channel + 1][frame]->wUp = weight;
				}
			}
		}
	}
}

void LEGION::findLeaders() {
	int leaders = 0;
	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {
			if (neuralGrid[channel][frame]->totalWeight() >= leaderNeighbors) {
				leaders++;
				neuralGrid[channel][frame]->makeLeader();
			}
		}
	}
	print "leaders: " << leaders endl;
}

Oscillator* LEGION::maxExcitement() {
	double max = -5;
	Oscillator* next = nullptr;
	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {
			if (neuralGrid[channel][frame]->isLeader() && neuralGrid[channel][frame]->excitement > max) {
				next = neuralGrid[channel][frame];
				max = neuralGrid[channel][frame]->excitement;
			}
		}
	}
	return next;
}

void LEGION::run() {
	bool spiked = true;
	do {
		Oscillator* next = maxExcitement();
		spiked = next->hasSpiked;
		if (!spiked) {
			int spikes = next->spike(numSegments);
			if (spikes < minGroup) {
				next->kill();
			}
			else {
				if (spikes > numInLargestSegment) {
					numInLargestSegment = spikes;
					largestSegment = numSegments;
				}
				allStep(spikes);
				numSegments++;
			}
		}
	} while (!spiked);
	print "segments: " << numSegments endl;
	saveSegmentGrid("LEGION.txt");
}

void LEGION::allStep(int spiked) {
	Segment* segment = new Segment(spiked);
	bool stepped = false;
	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {
			if (neuralGrid[channel][frame]->isSpiking()) {
				// store channel and frame in segment
				segment->add(new RowColumn(channel, frame));
				segmentGrid->set(channel, frame, neuralGrid[channel][frame]->segment);
				neuralGrid[channel][frame]->reset();
			}
			else {
				if (!stepped) {
					neuralGrid[channel][frame]->step();
					stepped = neuralGrid[channel][frame]->excitement >= SPIKE_THRESHOLD;
				}
			}
		}
	}
	if (segment->inserted < spiked) {
		print "NOT ENOUGH IN SEG\t" << spiked - segment->inserted endl;
	}
	(*segments).push_back(segment);
}

void LEGION::markLargestSegment() {
	//for (int channel = 0; channel < CHANNELS; channel++) {
	//	for (int frame = 0; frame < FRAMES; frame++) {
	//		if (neuralGrid[channel][frame]->segment == largestSegment) {
	//			neuralGrid[channel][frame]->largestSegment = true;
	//		}
	//	}
	//}
		//Segment seg = *(*segments)[largestSegment];
		//seg.largest = true;

}

boolGrid* LEGION::segmentsAsMask() {
	bool** grid = new bool*[CHANNELS];
	for (int channel = 0; channel < CHANNELS; channel++) {
		grid[channel] = new bool[FRAMES];
		for (int frame = 0; frame < FRAMES; frame++) {
			grid[channel][frame] = (*segmentGrid)(channel, frame) >= 0;
		}
	}
	return new boolGrid(grid, CHANNELS, FRAMES);
}


void LEGION::saveSegmentGrid(std::string location) {
	segmentGrid->toFile(location, 5);
}

LEGION::~LEGION()
{
	for (int i = 0; i < numSegments; i++) {
		(*segments)[i]->deleteSeg();
		delete ((*segments)[i]);
	}
	segments->clear();
	delete segments;

	for (int channel = 0; channel < CHANNELS; channel++) {
		for (int frame = 0; frame < FRAMES; frame++) {
			delete neuralGrid[channel][frame];
		}
		delete[] neuralGrid[channel];
	}
	delete[] neuralGrid;

	delete segmentGrid;
}
