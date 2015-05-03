#include "stdafx.h"
#include "testing.h"
#include "Oscillator(old).h"
#include "GlobalInhibitor.h"

namespace test{

	void testGammatoneFilter() {
		Signal* randomWave = staticTools::makeWave(44100, 44100, 100);
		GammatoneFilter filter(100, erb(100) * BW_CORRECTION, 44100);
		Signal* myFilterResults = filter.filter(*randomWave);
		double* otherFilterResults = mexFunction(randomWave->signal, 44100, 100, 44100);
		for (int i = 0; i < 44100; i++) {
			double my = (*myFilterResults)[i];
			double other = otherFilterResults[i];
			double diff = my - other;
			if (abs(diff) > .001) {
				print diff end;
			}
		}
	}


	void testMeddis() {
		Signal* signal = staticTools::makeWave(44100, 44100, 100);
		int sampleRate = 44100;
		int samples = 44100;
		int channels = 128;

		FilterBank bank(128, 80, 5500, sampleRate);
		Meddis meddis;
		SignalBank* basilarMembrane = bank.filter(*signal);
		SignalBank* cochleagram = meddis.filter(*basilarMembrane);
		double** correctCoch = meddis.unmodifiedFilter(*basilarMembrane, sampleRate, channels, samples);
		for (int i = 0; i < channels; i++) {
			for (int j = 0; j < samples; j++) {
				double diff = (*cochleagram)[i][j] - correctCoch[i][j];
				if (abs(diff) > .000001)
					print i << '\t' << j << '\t' << diff end;
			}
		}
	}

	void oscilTest() {
		OscillatorOld* dead = new OscillatorOld(.3, .5, .2, 0, 2);

		int size = 44100;
		double step = .1;
		OscillatorOld* oscil0 = new OscillatorOld(.8, .5, .003, -5, 2, size);
		OscillatorOld* oscil1 = new OscillatorOld(.3, .5, .003, 0, 2, size);
		OscillatorOld* oscil2 = new OscillatorOld(.5, .3, .001, .2, 2, size);
		OscillatorOld* oscil3 = new OscillatorOld(.2, .4, .002, .2, 2, size);
		OscillatorOld* oscil4 = new OscillatorOld(.1, 0, 0, 0, 2, size);
		GlobalInhibitor* gi = new GlobalInhibitor();
		for (int i = 0; i < size; i++) {
			oscil1->updatePotential(0 - 7.5, step);
			oscil2->updatePotential(((oscil3->excitement > -.8) + 0) - 1.5, step);
			oscil3->updatePotential(((oscil2->excitement > -.8) + (oscil4->excitement > -.8)) - 1.5, step);
			oscil4->updatePotential((oscil3->excitement > -.8) - 1.5, step);

			bool global = (oscil0->excitement > .5 || oscil1->excitement > .5 || oscil2->excitement > .5 || oscil3->excitement > .5 || oscil4->excitement > .5);// ? .5 : 0);
			gi->update(step, global);
			double globalInhibition = gi->inhibition > .5 ? .5 : 0;

			oscil0->updateNeighborWeights(new double[2]{(double)(oscil1->excitement > -.8), 0}, globalInhibition);
			oscil1->updateNeighborWeights(new double[2]{(double)(oscil0->excitement > -.8), 0}, globalInhibition);
			oscil2->updateNeighborWeights(new double[2]{((double)(oscil3->excitement > -.8)), 0}, globalInhibition);
			oscil3->updateNeighborWeights(new double[2]{((double)(oscil2->excitement > -.8)), ((double)(oscil4->excitement > -.8))}, globalInhibition);
			if (((double)(oscil3->excitement > -.8)) && ((double)(oscil4->excitement < -.8))) {
				int a = ((double)(oscil3->excitement > .5));
			}
			oscil4->updateNeighborWeights(new double[2]{((double)(oscil3->excitement > -.8)), 0}, globalInhibition);

			oscil0->update(step);
			oscil1->update(step);
			oscil2->update(step);
			oscil3->update(step);
			oscil4->update(step);


		}

		oscil0->saveWavs("oscil0");
		oscil1->saveWavs("oscil1");
		oscil2->saveWavs("oscil2");
		oscil3->saveWavs("oscil3");
		oscil4->saveWavs("oscil4");
	}
}