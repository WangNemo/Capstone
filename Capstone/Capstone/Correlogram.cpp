#include "stdafx.h"
#include "Correlogram.h"
#include "HanningWindow.h"

Correlogram::Correlogram(SignalBank& cochleagram, int frameSize, int frameOffset)
{
	print "correlating" endl;
	int frames = (cochleagram.SAMPLES - frameSize * cochleagram.SAMPLE_RATE / 1000) / (frameOffset * cochleagram.SAMPLE_RATE / 1000);
	print frames endl;
	int samplesPerFrame = (frameSize * cochleagram.SAMPLE_RATE / 1000);
	int lagSamples = (frameOffset *  cochleagram.SAMPLE_RATE) / 1000;
	
	T_FGrid = new SignalGrid(frames, cochleagram.CHANNELS, cochleagram.SAMPLES, cochleagram.SAMPLE_RATE, frameSize, frameOffset, frameSize - frameOffset);
	fundamentialFrequencyTime = new int[frames];
	HanningWindow han(samplesPerFrame);
	for (int f = 0; f < frames; f++) {
		SignalBank* frequencyBank = new SignalBank(cochleagram.CHANNELS, cochleagram.SAMPLE_RATE, cochleagram.SAMPLES);
		Signal summary(lagSamples, cochleagram.SAMPLE_RATE);
		for (int i = 0; i < cochleagram.CHANNELS; i++) {
			Signal* cor = cochleagram[i].autoCorrelate(frameOffset, f * frameOffset, *han.window, activityThreshold);// .99);
			frequencyBank->add(cor, i);
			for (int sample = 0; sample < lagSamples; sample++) {
				summary[sample] += (*cor)[sample];
			}
		}
		T_FGrid->addBank(frequencyBank, f);
		summary.normalize();
		fundamentialFrequencyTime[f] = summary.sampleOfHighestPeak(2);
	#ifdef _DEBUG
		print '\t' << f endl;
	#endif

	}
}

boolGrid* Correlogram::toBinaryMask() {
	bool** binaryMask = new bool*[T_FGrid->CHANNELS];
	for (int row = 0; row < T_FGrid->CHANNELS; row++) {
		binaryMask[row] = new bool[T_FGrid->FRAMES];
		for (int col = 0; col < T_FGrid->FRAMES; col++) {
			binaryMask[row][col] = (*T_FGrid)[col][row][0] > 0;
		}
	}
	boolGrid* idealBinaryMask1 = new boolGrid(binaryMask, T_FGrid->CHANNELS, T_FGrid->FRAMES);
	return idealBinaryMask1;
}

double getCorrelation(int sample, Signal& sig, Signal& window, int lag) {
	// returns a single point
	double correlation = 0;
	for (int i = 0; i < window.SAMPLES; i++) {
		correlation += sig[sample + i] * sig[sample + i + lag] * window[i];
		// This can be +/- i and +/- lag
		// This can be * window[i] and also * window[i +/- lag]
	}
	return correlation;
}


Correlogram::~Correlogram()
{
	delete T_FGrid;
	delete[] fundamentialFrequencyTime;
}
