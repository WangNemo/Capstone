#include "stdafx.h"
#include "Cochleagram.h"
#include "Meddis.h"


Cochleagram::Cochleagram(Signal& signal, int sampleRate)
{
	FilterBank* bank = new FilterBank(CHANNELS, MIN_FREQ, MAX_FREQ, sampleRate);
	

	Meddis* meddis = new Meddis();
	SignalBank* basilarMembrane = bank->filter(signal);

	//for (int i = 0; i < CHANNELS; i++) {
	//	(*basilarMembrane)[i].normalize();
	//}

	cochleagram = basilarMembrane;
	//for (int i = 0; i < CHANNELS; i++) {
	//	(*basilarMembrane)[i].reverse();
	//	(*basilarMembrane).add((*((*bank).bank[i])).filter((*basilarMembrane)[i]), i);
	//	(*basilarMembrane)[i].reverse();
	//}
	for (int i = 0; i < CHANNELS; i++) {
		(*cochleagram)[i].normalize();
		print minInArray((*cochleagram)[i].signal, signal.SAMPLES) << '\t' << maxInArray((*cochleagram)[i].signal, signal.SAMPLES) << '\t' << avgInArray((*cochleagram)[i].signal, signal.SAMPLES) end;

		(*basilarMembrane)[i].scale(100);
	}
	delete bank;
	cochleagram = meddis->filter(*basilarMembrane);
	for (int i = 0; i < CHANNELS; i++) {
		print minInArray((*cochleagram)[i].signal, signal.SAMPLES) << '\t' << maxInArray((*cochleagram)[i].signal, signal.SAMPLES) << '\t' << avgInArray((*cochleagram)[i].signal, signal.SAMPLES) end;
	}
	delete meddis;
	delete basilarMembrane;
}


Cochleagram::~Cochleagram()
{
	delete cochleagram;
}
