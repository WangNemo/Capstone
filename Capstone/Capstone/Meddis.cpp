#include "stdafx.h"
#include "Meddis.h"


SignalBank* Meddis::filter(SignalBank& bm)
{
	print "meddis" end;
	//% initialize inner hair cells
	double processAdj = PROCESS_RATE / bm.SAMPLE_RATE;
	double replenishAdj = REPLENISH_RATE / bm.SAMPLE_RATE;
	double returnAdj = RETURN_RATE / bm.SAMPLE_RATE;
	double permAdj = perm3 / bm.SAMPLE_RATE;
	double lossReturn = (LOSS_RATE + RETURN_RATE) / bm.SAMPLE_RATE;

	//% inner hair cell transduction
	SignalBank* meddisResult = new SignalBank(bm.CHANNELS, bm.SAMPLE_RATE, bm.SAMPLES);
	//SignalBank& meddisResult = *meddisResultPtr;
	for (int channel = 0; channel < bm.CHANNELS; channel++) {
		Signal* sig = new Signal(bm.SAMPLES, bm.SAMPLE_RATE);
		meddisResult->add(sig, channel);
		//Signal& signal = *sig;

		//meddisResult[channel] = new double[samples];
		double permability = perm3 * perm1 / (perm1 + perm2);
		double cleftContents = TRANSMITTERS * REPLENISH_RATE * permability / 
			(LOSS_RATE * permability + REPLENISH_RATE * (LOSS_RATE + RETURN_RATE));
		double transmitter = cleftContents * (LOSS_RATE + RETURN_RATE) / permability;
		double reprocessor = cleftContents *  RETURN_RATE / PROCESS_RATE;

		Signal& signal = bm[channel];
		Signal& result = (*meddisResult)[channel];
		for (int sample = 0; sample < bm.SAMPLES; sample++) {
			double sigPerm = (signal[sample] + perm1);
			double permabilityAmt = (sigPerm > 0 ?
				permAdj * sigPerm / (signal[sample] + perm1 + perm2)
				: 0);
			double replenishAmt = transmitter < TRANSMITTERS ?
				replenishAdj - replenishAdj * transmitter
				: 0;
			double ejectAmt = permabilityAmt * transmitter;
			double lossAmt = lossReturn * cleftContents;
			double returnAmt = returnAdj * cleftContents;
			double reprocessAmt = processAdj * reprocessor;

			transmitter = MAX(transmitter + replenishAmt - ejectAmt + reprocessAmt, 0);
			cleftContents = MAX(cleftContents + ejectAmt - lossAmt, 0);
			reprocessor = MAX(reprocessor + returnAmt - reprocessAmt, 0);

			result[sample] = cleftContents * FIRING_PROBABILITY;
		}
	}
	return meddisResult;
}

double** Meddis::unmodifiedFilter(SignalBank& bm, int sampleRate, int channels, int samples)
{
	//[numChan, sigLength] = size(r);     % number of channels and input signal length

	//% hair cell parameters
	double med_y = 5.05;
	double med_g = 2000;
	double med_l = 2500;
	double med_r = 6580;
	double med_x = 66.31;
	double med_a = 3.0;
	double med_b = 200;
	double med_h = 48000;
	double med_m = 1;

	//% initialize inner hair cells
	double ymdt = med_y*med_m / sampleRate;
	double xdt = med_x / sampleRate;
	double ydt = med_y / sampleRate;
	double lplusrdt = (med_l + med_r) / sampleRate;
	double rdt = med_r / sampleRate;
	double gdt = med_g / sampleRate;
	double hdt = med_h;

	//% inner hair cell transduction
	//hc = zeros(numChan, sigLength);
	double** meddisResult = new double*[channels];
	for (int i = 0; i < channels; i++) {
		meddisResult[i] = new double[samples];
		double kt = med_g*med_a / (med_a + med_b);
		double hair_c = med_m*med_y*kt / (med_l*kt + med_y*(med_l + med_r));
		double hair_q = hair_c*(med_l + med_r) / kt;
		double hair_w = hair_c*med_r / med_x;

		double* signal = bm[i].signal;
		double* result = meddisResult[i];
		for (int j = 0; j < samples; j++) {
			kt = ((signal[j] + med_a)>0 ?
				gdt*(signal[j] + med_a) / (signal[j] + med_a + med_b) :
				0);
			double replenish = hair_q < med_m ?
				replenish = ymdt - ydt*hair_q :
				replenish = 0;
			double eject = kt*hair_q;
			double reuptakeandloss = lplusrdt*hair_c;
			double reuptake = rdt*hair_c;
			double reprocess = xdt*hair_w;
			hair_q = MAX(hair_q + replenish - eject + reprocess, 0);
			hair_c = MAX(hair_c + eject - reuptakeandloss, 0);
			hair_w = MAX(hair_w + reuptake - reprocess, 0);
			result[j] = hair_c*hdt;
		}
	}
	return meddisResult;
}


Meddis::~Meddis()
{

}
