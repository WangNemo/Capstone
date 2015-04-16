// Capstone.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "definitions.h"

#include "testing.h"
#include "SignalBank.h"
#include "SignalGrid.h"

void main() {
	Signal* signal = staticTools::makeWave(44100, 100);
	int sampleRate = 44100;
	int samples = 44100;
	int channels = 128;

	FilterBank bank(128, 80, 5500, sampleRate);
	Meddis meddis;
	SignalBank* basilarMembrane = bank.filter(*signal);
	SignalBank* cochleagram = meddis.filter(*basilarMembrane);
	SignalGrid grid(*cochleagram, .020*sampleRate, .010*sampleRate);
	double** powerGrid = grid.toSmrPower();
}

