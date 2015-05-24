#include "stdafx.h"
#include "Cochleagram.h"
#include "Meddis.h"


Cochleagram::Cochleagram(Signal& signal, int sampleRate)
{
	FilterBank* bank = new FilterBank(CHANNELS, MIN_FREQ, MAX_FREQ, sampleRate);
	

	
	SignalBank* basilarMembrane = bank->filter(signal);

	/*for (int i = 0; i < CHANNELS; i++) {
		(*basilarMembrane)[i].normalize();
	}*/

	cochleagram = basilarMembrane;

	for (int i = 0; i < CHANNELS; i++) {
		//print minInArray((*cochleagram)[i].signal, signal.SAMPLES) << '\t' << maxInArray((*cochleagram)[i].signal, signal.SAMPLES) << '\t' << avgInArray((*cochleagram)[i].signal, signal.SAMPLES) endl;
		//(*cochleagram)[i].normalize();

		(*basilarMembrane)[i].scale(100);
	}
	delete bank;

	Meddis* meddis = new Meddis();
	cochleagram = meddis->filter(*basilarMembrane, CHANNELS - 1);
	for (int i = 0; i < CHANNELS; i++) {
		//print minInArray((*cochleagram)[i].signal, signal.SAMPLES) << '\t' << maxInArray((*cochleagram)[i].signal, signal.SAMPLES) << '\t' << avgInArray((*cochleagram)[i].signal, signal.SAMPLES) endl;
		(*cochleagram)[i].zeroMeanStandardVariance();
	}
	delete meddis;
	delete basilarMembrane;
}


Cochleagram::~Cochleagram()
{
	delete cochleagram;
}
