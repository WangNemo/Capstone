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
	for (int i = 0; i < CHANNELS; i++) {
		(*basilarMembrane)[i].scale(100);
	}
	delete bank;
	cochleagram = meddis->filter(*basilarMembrane);
	for (int i = 0; i < CHANNELS; i++) {
		print minInArray((*cochleagram)[i].signal, CHANNELS) << '\t' << maxInArray((*cochleagram)[i].signal, CHANNELS) << '\t' << avgInArray((*cochleagram)[i].signal, CHANNELS) end;
	}
	delete meddis;
	delete basilarMembrane;
}


Cochleagram::~Cochleagram()
{
	delete cochleagram;
}
