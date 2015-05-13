#include "stdafx.h"
#include "SignalGrid.h"
#include "FilterBank.h"
#include "Cochleagram.h"


SignalBank& SignalGrid::operator[](int frame) {
	return *(grid[frame]);
}

SignalGrid::SignalGrid(SignalBank& signalBank, int frameSize, int frameOverlap)
	: FRAME_OFFSET(frameSize - frameOverlap),
	FRAME_SIZE(frameSize), FRAME_OVERLAP(frameOverlap),	SAMPLES(signalBank.SAMPLES), 
	SAMPLE_RATE(signalBank.SAMPLE_RATE), CHANNELS(signalBank.CHANNELS)
{

	//FRAMES = FRAME_OFFSET == 0 ? SAMPLES / FRAME_SIZE : (SAMPLES - FRAME_SIZE) / FRAME_OFFSET + 1;
	FRAMES = (SAMPLES - frameSize) / FRAME_OFFSET;
	grid = new SignalBank*[FRAMES];
	HanningWindow window(frameSize);

	for (int frame = 0; frame < FRAMES; frame++) {
		SignalBank* bank = new SignalBank(CHANNELS, SAMPLE_RATE, SAMPLES);
		for (int channel = 0; channel < CHANNELS; channel++) {
			Signal* sig = new Signal(FRAME_SIZE, SAMPLE_RATE);
			for (int sample = 0; sample < FRAME_SIZE; sample++) {
				(*sig)[sample] = signalBank[channel][sample + FRAME_OFFSET * frame] * window[sample];
			}
			bank->add(sig, channel);
		}
		grid[frame] = bank;
	}
}

SignalGrid::SignalGrid(int frames, int channels, int samples, int sampleRate, int frameSize, int overlap, int offset)
	: FRAMES(frames), CHANNELS(channels), SAMPLES(samples), SAMPLE_RATE(sampleRate),
	FRAME_SIZE(frameSize), FRAME_OFFSET(offset), FRAME_OVERLAP(overlap){
	grid = new SignalBank*[FRAMES] {};
}

void SignalGrid::addBank(SignalBank* bank, int index) {
	if (grid[index] != nullptr) {
		delete grid[index];
	}
	grid[index] = bank;
}


doubleGrid* SignalGrid::toSmrPower() {
	double** powerGrid = new double*[CHANNELS];
	for (int channel = 0; channel < CHANNELS; channel++){
		double* powerColumn = new double[FRAMES];
		powerGrid[channel] = powerColumn;
		for (int frame = 0; frame < FRAMES; frame++) {
			double power = (*(grid[frame]))[channel].squareSum();// 0;
			//for (int i = 0; i < FRAME_SIZE; i++) {
			//	double sample = (*(grid[frame]))[channel][i];
			//	power += sample * sample;
			//}
			
			powerColumn[frame] = sqrt(power);
		}
	}
	/*for (int i = 0; i < FRAMES; i++) {
		print minInArray(powerGrid[i], CHANNELS) << '\t' << maxInArray(powerGrid[i], CHANNELS) << '\t' << avgInArray(powerGrid[i], CHANNELS) endl;
	}*/
	return new doubleGrid(powerGrid, CHANNELS, FRAMES);
}

void SignalGrid::deleteCell(int frame, int channel) {
	(*(grid[frame]))[channel].zeroOut();
}

Signal* SignalGrid::resynthesize(boolGrid& mask) {
	for (int channel = 0; channel < mask.ROWS; channel++) {
		for (int frame = 0; frame < mask.COLUMNS; frame++) {
			//mixedGrid[frame][channel].scale((*decibelGrid)(frame, channel));
			if (!(mask)(channel, frame))
				deleteCell(frame, channel);
			/*else
				print "frame: " << frame << "\tchannel: " << channel endl;*/
		}
	}
	for (int frame = mask.COLUMNS; frame < FRAMES; frame++) {
		for (int channel = mask.ROWS; channel < CHANNELS; channel++) {
			deleteCell(frame, channel);
		}
	}

	SignalBank* resynthesisBank = new SignalBank(CHANNELS, SAMPLE_RATE, SAMPLES);
	for (int i = 0; i < CHANNELS; i++) {
		resynthesisBank->add(new Signal(SAMPLES, SAMPLE_RATE), i);
	}

	for (int frame = 0; frame < FRAMES; frame++) {
		for (int channel = 0; channel < CHANNELS; channel++) {
			for (int sample = 0; sample < FRAME_SIZE; sample++) {
				(*resynthesisBank)[channel][sample + FRAME_OFFSET * frame] += (*this)[frame][channel][sample];
			}
		}
	}

	Signal* resynthesized = new Signal(SAMPLES, resynthesisBank->SAMPLE_RATE);
	for (int channel = 0; channel < CHANNELS; channel++){//=2) {
		for (int sample = 0; sample < SAMPLES; sample++) {
			((*resynthesized)[sample]) += (*resynthesisBank)[channel][sample];
		}
	}
	delete resynthesisBank;
	//resynthesized->normalize();
	return resynthesized;
}

Signal* SignalGrid::resynthesize(Signal& signal, int frames, int channels, int samples, int sampleRate, int frameSize, int overlap){
	FilterBank bank(channels, Cochleagram::MIN_FREQ, Cochleagram::MAX_FREQ, 44100);
	SignalBank* siggyb = bank.filter(signal);
	SignalGrid* siggyGrid = new SignalGrid(*siggyb, frameSize, overlap);
	bool** bools = new bool*[channels];
	for (int channel = 0; channel < channels; channel++) {
		bools[channel] = new bool[siggyGrid->FRAMES];
		std::fill_n(bools[channel], siggyGrid->FRAMES, true);
	}
	boolGrid* mask = new boolGrid(bools, channels, siggyGrid->FRAMES);

	Signal* resyth = siggyGrid->resynthesize(*mask);

	delete siggyb;
	delete siggyGrid;
	delete mask;

	return resyth;
}




SignalGrid::~SignalGrid()
{
	for (int i = 0; i < FRAMES; i++) {
		delete grid[i];
	}
	delete[] grid;
}
