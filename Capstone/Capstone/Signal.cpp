#include "stdafx.h"
#include "Signal.h"
#include <array>

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
	double largest = 0;
	for (int sample = 0; sample < SAMPLES; sample++) {
		double sam = signal[sample];
		if (sam > largest) {
			largest = sam;
		}
	}

	for (int sample = 0; sample < SAMPLES; sample++) {
		double sam = signal[sample];
		signal[sample] = sam / largest;
	}
}


Signal::~Signal(){
	delete[] signal;
}
