// Capstone.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "definitions.h"
#include "AudioFile.h"
#include "FilterBank.h"


#define erb(x)         ( 24.7 * ( 4.37e-3 * ( x ) + 1.0 ) )
#define BW_CORRECTION      1.0190


void main() {
	double* randomWave = staticTools::makeWave(44100, 100);
	GammatoneFilter filter(100, erb(100) * BW_CORRECTION, 44100);
	double* myFilterResults = filter.filter(randomWave, 44100);
	double* otherFilterResults = mexFunction(randomWave, 44100, 100, 44100);
	for (int i = 0; i < 44100; i++) {
		double my = myFilterResults[i];
		double other = otherFilterResults[i];
		double diff =  my - other;
		if (abs(diff) > .01) {
			print diff end;
		}
	}
}

