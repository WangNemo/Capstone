#include "stdafx.h"
#include "GammatoneFilter.h"
#include <cmath>


GammatoneFilter::GammatoneFilter(int frequencyCenter, double bandwidth, int sampleRate)
{
	GammatoneFilter::frequencyCenter = frequencyCenter;

	//GammatoneFilter::radianFrequency = PI * 2 * frequencyCenter;
	GammatoneFilter::radianTimeStep = (PI * 2) / sampleRate;
	GammatoneFilter::radianSize = radianTimeStep * bandwidth;

	double e = exp(-radianSize);
	GammatoneFilter::exp2 = e * e;
	//gain = (tptbw*tptbw*tptbw*tptbw) / 3;
	GammatoneFilter::gain = pow(radianSize, ORDER) / 3;// (ORDER + ORDER / 2);
	
	GammatoneFilter::orders[0] = ORDER * e;
	GammatoneFilter::orders[1] = -(ORDER + ORDER / 2)* exp2;
	GammatoneFilter::orders[2] = ORDER * exp2 * e;
	GammatoneFilter::orders[3] = -exp2 * exp2;

	GammatoneFilter::cosFreq = cos(radianTimeStep * frequencyCenter);
	GammatoneFilter::sinFreq = sin(radianTimeStep * frequencyCenter);
}

void GammatoneFilter::filterComplex(Complex& shiftedSample) {
	for (int i = 0; i < ORDER; i++) {
		shiftedSample.real += GammatoneFilter::orders[i] *
			GammatoneFilter::priorResults[i].real;
		shiftedSample.imaginary += GammatoneFilter::orders[i] *
			GammatoneFilter::priorResults[i].imaginary;
	}

	shiftPriorResults(shiftedSample);

	shiftedSample.real +=
		GammatoneFilter::orders[0] * GammatoneFilter::priorResults[0].real
		+ GammatoneFilter::exp2 * GammatoneFilter::priorResults[1].real;

	shiftedSample.imaginary +=
		GammatoneFilter::orders[0] * GammatoneFilter::priorResults[0].imaginary
		+ GammatoneFilter::exp2 * GammatoneFilter::priorResults[1].imaginary;
}

Signal* GammatoneFilter::filter(Signal& signal, bool envelope) {
	//print frequencyCenter << '\t' << radianSize / ((PI * 2) / signal.SAMPLE_RATE) endl;
	Signal* filteredSignal = new Signal(signal.SAMPLES, signal.SAMPLE_RATE);//new double[samples];

	Complex freqShift(1, 0);

	for (int sample = 0; sample < signal.SAMPLES; sample++) {
		Complex* filteredSamplePtr = new Complex();

		Complex filteredSample = *filteredSamplePtr;
		// shift signal's frequency to center
		filteredSample.real = freqShift.real * signal[sample];
		filteredSample.imaginary = freqShift.imaginary * signal[sample];

		filterComplex(filteredSample);

		// shift signal back
		double shiftedBack = /*envelope ?
				sqrt(filteredSample.real * filteredSample.real +
				filteredSample.imaginary * filteredSample.imaginary)
			:*/
				filteredSample.real * freqShift.real +
				filteredSample.imaginary * freqShift.imaginary;

		delete filteredSamplePtr;

		(*filteredSignal)[sample] = shiftedBack * GammatoneFilter::gain;

		double tempCos = freqShift.real;
		freqShift.real = cosFreq * tempCos + sinFreq * freqShift.imaginary;
		freqShift.imaginary = cosFreq * freqShift.imaginary - tempCos * sinFreq;
	}
	return filteredSignal;
}

void GammatoneFilter::shiftPriorResults(Complex& newResult) {
	for (int i = ORDER - 1; i > 0; i--) {
		GammatoneFilter::priorResults[i] = GammatoneFilter::priorResults[i - 1];
	}
	GammatoneFilter::priorResults[0] = newResult;
}

GammatoneFilter::~GammatoneFilter()
{
}
