#include "stdafx.h"
#include "testing.h"

namespace test{

	void testGammatoneFilter() {
		double* randomWave = staticTools::makeWave(44100, 100);
		GammatoneFilter filter(100, erb(100) * BW_CORRECTION, 44100);
		double* myFilterResults = filter.filter(randomWave, 44100);
		double* otherFilterResults = mexFunction(randomWave, 44100, 100, 44100);
		for (int i = 0; i < 44100; i++) {
			double my = myFilterResults[i];
			double other = otherFilterResults[i];
			double diff = my - other;
			if (abs(diff) > .001) {
				print diff end;
			}
		}
	}


	void testMeddis() {
		double* signal = staticTools::makeWave(44100, 100);
		int sampleRate = 44100;
		int samples = 44100;
		int channels = 128;

		FilterBank bank(128, 80, 5500, sampleRate);
		Meddis meddis;
		double** basilarMembrane = bank.filter(signal, samples);
		double** cochleagram = meddis.filter(basilarMembrane, sampleRate, channels, samples);
		double** correctCoch = meddis.unmodifiedFilter(basilarMembrane, sampleRate, channels, samples);
		for (int i = 0; i < channels; i++) {
			for (int j = 0; j < samples; j++) {
				double diff = cochleagram[i][j] - correctCoch[i][j];
				if (abs(diff) > .000001)
					print i << '\t' << j << '\t' << diff end;
			}
		}
	}
}