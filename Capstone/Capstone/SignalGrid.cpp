#include "stdafx.h"
#include "SignalGrid.h"


SignalBank& SignalGrid::operator[](int frame) {
	return *(grid[frame]);
}

SignalGrid::SignalGrid(SignalBank& signalBank, int frameSize, int frameOverlap)
	: FRAME_OFFSET(frameSize - frameOverlap),
	FRAME_SIZE(frameSize), FRAME_OVERLAP(frameOverlap),	SAMPLES(signalBank.SAMPLES), 
	SAMPLE_RATE(signalBank.SAMPLE_RATE), CHANNELS(signalBank.CHANNELS)
{
	FRAMES = (SAMPLES - FRAME_SIZE) / FRAME_OFFSET + 1;
	grid = new SignalBank*[FRAMES];

	for (int frame = 0; frame < FRAMES; frame++) {
		SignalBank* bank = new SignalBank(CHANNELS, SAMPLE_RATE, SAMPLES);
		for (int channel = 0; channel < CHANNELS; channel++) {
			Signal* sig = new Signal(FRAME_SIZE);
			for (int sample = 0; sample < FRAME_SIZE; sample++) {
				(*sig)[sample] = signalBank[channel][sample + FRAME_OFFSET * frame];
			}
			bank->add(sig, channel);
		}
		grid[frame] = bank;
	}
	print FRAMES end;
}

double** SignalGrid::toSmrPower() {
	double** powerGrid = new double*[FRAMES];
	for (int frame = 0; frame < FRAMES; frame++) {
		double* powerColumn = new double[CHANNELS];
		for (int channel = 0; channel < CHANNELS; channel++){
			double power = 0;
			for (int i = 0; i < FRAME_SIZE; i++) {
				double sample = (*(grid[frame]))[channel][i];
				power += sample * sample;
			}
			powerColumn[channel] = sqrt(power);
		}
	}
	return powerGrid;
}


SignalGrid::~SignalGrid()
{
}
