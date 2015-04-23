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

	FRAMES = FRAME_OFFSET == 0 ? SAMPLES / FRAME_SIZE : (SAMPLES - FRAME_SIZE) / FRAME_OFFSET + 1;
	//FRAMES -= 5;
	grid = new SignalBank*[FRAMES];
	HanningWindow window(frameSize);

	for (int frame = 0; frame < FRAMES; frame++) {
		SignalBank* bank = new SignalBank(CHANNELS, SAMPLE_RATE, SAMPLES);
		for (int channel = 0; channel < CHANNELS; channel++) {
			Signal* sig = new Signal(FRAME_SIZE, SAMPLE_RATE);
			for (int sample = 0; sample < FRAME_SIZE; sample++) {
				(*sig)[sample] = signalBank[channel][sample + FRAME_OFFSET * frame] * (*window.window)[sample];
			}
			bank->add(sig, channel);
		}
		grid[frame] = bank;
	}
}

doubleGrid* SignalGrid::toSmrPower() {
	double** powerGrid = new double*[FRAMES];
	for (int frame = 0; frame < FRAMES; frame++) {
		double* powerColumn = new double[CHANNELS];
		powerGrid[frame] = powerColumn;
		for (int channel = 0; channel < CHANNELS; channel++){
			double power = 0;
			for (int i = 0; i < FRAME_SIZE; i++) {
				double sample = (*(grid[frame]))[channel][i];
				power += sample * sample;
			}
			powerColumn[channel] = sqrt(power);
		}
	}
	/*for (int i = 0; i < FRAMES; i++) {
		print minInArray(powerGrid[i], CHANNELS) << '\t' << maxInArray(powerGrid[i], CHANNELS) << '\t' << avgInArray(powerGrid[i], CHANNELS) end;
	}*/
	return new doubleGrid(powerGrid, FRAMES, CHANNELS);
}

void SignalGrid::deleteCell(int frame, int channel) {
	(*(grid[frame]))[channel].zeroOut();
}

Signal* SignalGrid::resynthesize(boolGrid& mask) {
	for (int frame = 0; frame < mask.ROWS; frame++) {
		for (int channel = 0; channel < mask.COLUMNS; channel++) {
			//mixedGrid[frame][channel].scale((*decibelGrid)(frame, channel));
			if (!(mask)(frame, channel))
				deleteCell(frame, channel);
		}
	}
	for (int frame = mask.ROWS; frame < FRAMES; frame++) {
		for (int channel = mask.COLUMNS; channel < CHANNELS; channel++) {
			deleteCell(frame, channel);
		}
	}

	SignalBank resynthesisBank(CHANNELS, SAMPLE_RATE, SAMPLES);
	for (int i = 0; i < CHANNELS; i++) {
		resynthesisBank.add(new Signal(SAMPLES, SAMPLE_RATE), i);
	}

	for (int frame = 0; frame < FRAMES; frame++) {
		for (int channel = 0; channel < CHANNELS; channel++) {
			for (int sample = 0; sample < FRAME_SIZE; sample++) {
				resynthesisBank[channel][sample + FRAME_OFFSET * frame] += (*this)[frame][channel][sample];
			}
		}
	}

	Signal* resynthesized = new Signal(SAMPLES, resynthesisBank.SAMPLE_RATE);
	for (int channel = 0; channel < CHANNELS; channel++){//=2) {
		for (int sample = 0; sample < SAMPLES; sample++) {
			((*resynthesized)[sample]) += resynthesisBank[channel][sample];
		}
	}

	resynthesized->normalize();
	return resynthesized;
}




SignalGrid::~SignalGrid()
{
	for (int i = 0; i < FRAMES; i++) {
		delete grid[i];
	}
}
