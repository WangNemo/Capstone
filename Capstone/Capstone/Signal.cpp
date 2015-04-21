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
		double sam = signal[sample];
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

Signal* Signal::minus(Signal& other) {
	int length = MAX(other.SAMPLES, SAMPLES);
	Signal* s = new Signal(length, SAMPLE_RATE);
	for (int i = 0; i < length; i++) {
		(*s).signal[i] = signal[i] - other[i];
	}
	return s;
}


Signal::~Signal(){
	delete[] signal;
}
