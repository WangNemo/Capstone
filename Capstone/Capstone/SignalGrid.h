#pragma once
#include "Definitions.h"
#include "SignalBank.h"

class SignalGrid
{
private:
	SignalBank** grid;
public:
	const int FRAME_SIZE, FRAME_OVERLAP, FRAME_OFFSET, SAMPLES, CHANNELS, SAMPLE_RATE;
	int FRAMES;

	SignalBank& operator[](int frame);
	SignalGrid(SignalBank& signalBank, int samplesPerFrame, int samplesFrameOverlap);
	doubleGrid* toSmrPower();
	void deleteCell(int frame, int channel);
	~SignalGrid();
};

