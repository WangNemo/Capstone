#pragma once
#include "Definitions.h"
#include "SignalBank.h"
#include "HanningWindow.h"

class SignalGrid
{
private:
	SignalBank** grid;
public:
	const int FRAME_SIZE, FRAME_OVERLAP, FRAME_OFFSET, SAMPLES, CHANNELS, SAMPLE_RATE;
	int FRAMES;

	SignalBank& operator[](int frame);
	SignalGrid(SignalBank& signalBank, int samplesPerFrame, int samplesFrameOverlap);
	SignalGrid(int frames, int channels, int samples, int sampleRate, int frameSize, int overlap, int offset);
	void addBank(SignalBank* bank, int index);
	doubleGrid* toSmrPower();
	void deleteCell(int frame, int channel);
	Signal* resynthesize(boolGrid& mask);
	static Signal* SignalGrid::resynthesize(Signal& signal, int frames, int channels, int samples, int sampleRate, int frameSize, int overlap);
	~SignalGrid();
};

