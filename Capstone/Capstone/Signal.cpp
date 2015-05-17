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
	
	normalize(getMax());

}

void Signal::normalize(double max) {
	for (int sample = 0; sample < SAMPLES && max > 0; sample++) {
		double sam = signal[sample];
		signal[sample] = sam / max;
	}
}

void Signal::zeroMeanStandardVariance() {

	double average = avgInArray(signal, SAMPLES);

	if (abs(average) > .01) {
		for (int i = 0; i < SAMPLES; i++) {
			signal[i] -= average;
		}
	}

	double standardDerivation = staticTools::standardDeviation(*this, average);
	if (abs(standardDerivation) > .01) {
		for (int i = 0; i < SAMPLES; i++) {
			signal[i] /= standardDerivation;
		}
	}

}

void Signal::scale(double scalar) {
	for (int sample = 0; sample < SAMPLES; sample++) {
		double sam = signal[sample];
		signal[sample] = sam * scalar;
	}
}


void Signal::trim(int samples) {
	SAMPLES = samples;
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

void Signal::save(std::string& path) {
	FILE* file = fopen(path.c_str(), "wb");
	fwrite(signal, sizeof(double), SAMPLES, file);
	fclose(file);
}


Signal* Signal::autoCorrelate(int lagMS, int startingMS, Signal& window, double threshold) {
	int lagSamples = (lagMS * SAMPLE_RATE) / 1000;
	int startingSample = (startingMS * SAMPLE_RATE) / 1000;
	double noTimeDelay = getCorrelation(startingSample, window, 0);
	Signal* correlation = new Signal(lagSamples, SAMPLE_RATE);

	if (noTimeDelay < threshold) {
		//print noTimeDelay endl;
		return correlation;
	}

	(*correlation)[0] = noTimeDelay;
	for (int lag = 1; lag < lagSamples; lag++) {
		(*correlation)[lag] = getCorrelation(startingSample, window, lag);
	}
	correlation->zeroMeanStandardVariance();
	return correlation;
}
double Signal::crossCorrelate(Signal& other) {
	if (signal[0] == 0 || other[0] == 0)
		return 0;

	double sum = 0;
	for (int i = 0; i < SAMPLES - 1; i++) {
		sum += signal[i] * other[i + 1];
	}
	return sum / SAMPLES;
}

double Signal::getCorrelation(int startingSample, Signal& window, int lag) {
	double sample = 0;
	for (int i = 0; i < window.SAMPLES; i++) {
		sample += signal[i + startingSample] * signal[startingSample + i + lag] /** window[i]*/;
	}
	return sample;
}

double Signal::squareSum() {
	double power = 0;
	for (int i = 0; i < SAMPLES; i++) {
		double sample = signal[i];
		power += sample * sample;
	}
	return power;
}

double Signal::getMax() {
	double largest = -1000000;
	for (int sample = 0; sample < SAMPLES; sample++) {
		double sam = abs(signal[sample]);
		if (sam > largest) {
			largest = sam;
		}
	}
	return largest;
}

int Signal::sampleOfHighestPeak(int millis) {
	int start = (millis * SAMPLE_RATE) / 1000;
	double max = -100;
	int sample = -1;
	for (int i = start; i < SAMPLES; i++) {
		if (signal[i] > max) {
			max = signal[i];
			sample = i;
		}
	}
	return sample;
}



Signal::~Signal(){
	delete[] signal;
}
