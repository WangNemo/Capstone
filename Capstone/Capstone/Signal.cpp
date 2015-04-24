#include "stdafx.h"
#include "Signal.h"
#include <array>

#include "Definitions.h"

double& Signal::operator[](int sample) {
	return signal[sample];
}

Signal::Signal(int samples, int sampleRate) : SAMPLES(samples), SAMPLE_RATE(sampleRate) {
	signal = new double[samples] {};
}

void Signal::zeroOut() {
	std::fill_n(signal, SAMPLES, 0);
}

void Signal::reverse() {
	for (int i = 0; i < SAMPLES / 2; i++) {
		int reversei = SAMPLES - (i+1);
		double temp = signal[i];
		signal[i] = signal[reversei];
		signal[reversei] = temp;
	}
}

void Signal::normalize() {
	double largest = -1000000;
	for (int sample = 0; sample < SAMPLES; sample++) {
		double sam = abs(signal[sample]);
		if (sam > largest) {
			largest = sam;
		}
	}
	//print largest end;
	for (int sample = 0; sample < SAMPLES; sample++) {
		double sam = signal[sample];
		signal[sample] = sam / largest;
	}
}

void Signal::scale(double scalar) {
	for (int sample = 0; sample < SAMPLES; sample++) {
		double sam = signal[sample];
		signal[sample] = sam * scalar;
	}
}


void Signal::trim(int samples) {
	for (int sample = samples; sample < SAMPLES; sample++) {
		signal[sample] = 0;
	}
}

void Signal::add(Signal& other) {
	for (int i = 0; i < SAMPLES; i++) {
		signal[i] += other[i];
	}
}


Signal* Signal::minus(Signal& other) {
	int length = MAX(other.SAMPLES, SAMPLES);
	Signal* s = new Signal(length, SAMPLE_RATE);
	for (int i = 0; i < length; i++) {
		(*s).signal[i] = signal[i] - other[i];
	}
	return s;
}

Signal* Signal::autoCorrelate(int lagMS, int startingMS, Signal& window, double threshold) {
	int lagSamples = (lagMS * SAMPLE_RATE) / 1000;
	int startingSample = (startingMS * SAMPLE_RATE) / 1000;
	double noTimeDelay = getCorrelation(startingSample, window, 0);
	Signal* correlation = new Signal(lagSamples, SAMPLE_RATE);

	if (noTimeDelay < threshold) {
		print noTimeDelay end;
		return correlation;
	}

	(*correlation)[0] = noTimeDelay;
	for (int lag = 1; lag < lagSamples; lag++) {
		(*correlation)[lag] = getCorrelation(startingSample, window, lag);
	}
	return correlation;
}

double Signal::getCorrelation(int startingSample, Signal& window, int lag) {
	double sample = 0;
	for (int i = 0; i < window.SAMPLES; i++) {
		sample += signal[i + startingSample] * signal[startingSample + i + lag] * window[i];
	}
	return sample;
}



Signal::~Signal(){
	delete[] signal;
}
