// Capstone.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "definitions.h"
#include<fstream>

#include "testing.h"
#include "SignalBank.h"
#include "SignalGrid.h"
#include "GammatoneFilter.h"
#include "Correlogram.h"
#include "crossCorrelationSegmentation.h"
#include "IdealBinaryMask.h"
#include "Oscillator.h"
#include <ctime>
#include "LEGION.h"

int main(int argc, char* argv[], char* envp[]) {
	srand(static_cast <unsigned> (time(0)));


	print argv[0] end;
	print argv[1] end;
	print argv[2] end;
	std::string fileName(argv[1]);
	std::string fileName2(argv[2]);
	Signal* signal1 = staticTools::readWav(fileName);


	Signal* signal2 = staticTools::readWav(fileName2);
	int shorter = MIN(signal2->SAMPLES, signal1->SAMPLES);
	signal1->trim(shorter);
	signal2->trim(shorter);

	Signal* mixed = staticTools::combine(*signal1, *signal2);
	mixed->trim(44100);
	mixed->normalize();
	FILE* mixF = fopen("Mixed.wav", "wb");
	fwrite(mixed->signal, sizeof(double), mixed->SAMPLES, mixF);

	double total = 0;
	double low = 0;
	double max = 0;
	for (int i = 0; i < 10000000; i++) {
		double rand = staticTools::nonZeroGaussianRandom(0);
		total += rand;
		if (rand < low) low = rand;
		if (rand > max) max = rand;
	}

	//Oscillator* dead = new Oscillator(.3, .5, .2, 0, 2);

	//Cochleagram displaydsfa(*mixed, mixed->SAMPLE_RATE);
	//Correlogram coresdfa(*displaydsfa.cochleagram, 20, 10);
	int size = 44100;
	double step = .1;
	Oscillator* oscil1 = new Oscillator(.3, .5, .003, 0, 2, size);
	Oscillator* oscil2 = new Oscillator(.5, .3, .001, .2, 1, size);
	Oscillator* oscil3 = new Oscillator(.2, .4, .002, .2, 2, size);
	Oscillator* oscil4 = new Oscillator(.1, 0, 0, .2, 1, size);
	GlobalInhibitor* gi = new GlobalInhibitor();
	for (int i = 0; i < size; i++) {
		//oscil1->updatePotential(0 - 7.5, step);
		//oscil2->updatePotential(((oscil3->excitement > .5) + 0) - 1.5, step);
		//oscil3->updatePotential(((oscil2->excitement > .5) + (oscil4->excitement > .5)) - 1.5, step);
		//oscil4->updatePotential((oscil3->excitement > .5) - 1.5, step);

		bool global = (oscil1->excitement > .5 || oscil2->excitement > .5 || oscil3->excitement > .5 || oscil4->excitement > .5);// ? .5 : 0);
		gi->update(step, global);

		//oscil1->updateNeighborWeights(new double[2]{0, 0}, gi ->inhibition);
		//oscil2->updateNeighborWeights(new double[2]{((double)(oscil3->excitement > .5)), 0}, gi->inhibition);
		//oscil3->updateNeighborWeights(new double[2]{((double)(oscil2->excitement > .5)), ((double)(oscil4->excitement > .5))}, gi->inhibition);
		//oscil4->updateNeighborWeights(new double[2]{((double)(oscil3->excitement > .5)), 0}, gi->inhibition);

		oscil1->update(step);
		oscil2->update(step);
		oscil3->update(step);
		oscil4->update(step);


	}

	oscil1->saveWavs("oscil1");
	oscil2->saveWavs("oscil2");
	oscil3->saveWavs("oscil3");
	oscil4->saveWavs("oscil4");


	//LEGION* l = new LEGION(*coresdfa.T_FGrid);
	//l->run(60);

	return 0;
	//for (int i = 7; i >= 0; i--) {
	//	FILE* mixF = fopen(("corr" + std::to_string(8 - i)+  ".wav").c_str(), "wb");
	//	fwrite((*coresdfa.T_FGrid)[97][i].signal, sizeof(double), (*coresdfa.T_FGrid)[97][i].SAMPLES, mixF);
	//}

	IdealBinaryMask mask(*signal1, *signal2);
	std::fstream of1("ibm1.txt", std::ios::out);
	mask.writeBinaryMask(of1, mask.idealBinaryMask1);
	std::fstream of2("ibm2.txt", std::ios::out);
	mask.writeBinaryMask(of2, mask.idealBinaryMask2);
	//mask.saveIdealBinaryMask("testMask2.wav", mask.idealBinaryMask2);
	//mask.saveIdealBinaryMask("testMask1.wav", mask.idealBinaryMask1);

	return 0;
	//IdealBinaryMask mask(*signal1, *signal2);
	//std::fstream of1("ibm1.txt", std::ios::out);
	//mask.writeBinaryMask(of1, mask.idealBinaryMask1);
	//std::fstream of2("ibm2.txt", std::ios::out);
	//mask.writeBinaryMask(of2, mask.idealBinaryMask2);

	///*mask.saveIdealBinaryMask("testMask2.wav", mask.idealBinaryMask2);
	//mask.saveIdealBinaryMask("testMask1.wav", mask.idealBinaryMask1);*/

	//return 0;


	//
	//FILE* mixF = fopen("Mixed.wav", "wb");
	//fwrite(mixed->signal, sizeof(double), mixed->SAMPLES, mixF);

	Cochleagram* cochMix = new Cochleagram(*mixed, mixed->SAMPLE_RATE);
	//signal1->trim(4410);
	//Cochleagram* sigMix = new Cochleagram(*signal1, sampleRate);
	Correlogram* correl = new Correlogram(*cochMix->cochleagram, 20, 10);
	print correl->T_FGrid->FRAMES end;
	crossCorrelationSegmentation segmentation(*correl->T_FGrid);
	
	//boolGrid* idealBinaryMask = correl->toBinaryMask();
	//return 0;

	////Signal* corr = mixed->autoCorrelate(100);
	////corr->normalize();
	////FILE* corrTest = fopen("corrTest.wav", "wb");
	////fwrite(corr->signal, sizeof(double), corr->SAMPLES, corrTest);

	//return 0;

	//Cochleagram* cochMix = new Cochleagram(*mixed, signal1.SAMPLE_RATE);




	std::fstream of("Map.txt", std::ios::out);
	segmentation.writeSegmentText(of);

	//print "groups: " << segmentation.groups end;
	////SignalBank resynthBank(segmentation.groups - 1, mixedBank->SAMPLE_RATE, mixedBank->SAMPLES);
	//for (int group = 1; group < segmentation.groups - 1; group++) {
	//	print group end;
	//	boolGrid* idealBinaryMask = segmentation.getBinaryMask(group);

	//	SignalGrid mixedGrid = SignalGrid(*mixedBank, .020*sampleRate, .010*sampleRate);
	//	Signal* resynthesized = mixedGrid.resynthesize(*idealBinaryMask);

	//	FILE* results = fopen(("64group" + std::to_string(9999-group) + ".wav").c_str(), "wb");
	//	fwrite(resynthesized->signal, sizeof(double), resynthesized->SAMPLES, results);
	//	fclose(results);
	//	//resynthBank.add(resynthesized, group - 1);
	//	delete idealBinaryMask;
	//	delete resynthesized;
	//}


	//SignalBank resynthBank(segmentation->ROWS, mixedBank->SAMPLE_RATE, mixedBank->SAMPLES);
	//for (int group = 1; group < segmentation.groups - 1; group++) {
	//	print group end;
	//	boolGrid* idealBinaryMask = segmentation.getBinaryMask(group);

	//	SignalGrid mixedGrid = SignalGrid(*mixedBank, .020*sampleRate, .010*sampleRate);
	//	Signal* resynthesized = mixedGrid.resynthesize(*idealBinaryMask);

	//	resynthBank.add(resynthesized, group - 1);
	//	//delete idealBinaryMask;
	//	//delete resynthesized;
	//}

	//Signal result(mixedBank->SAMPLES, 44100);
	//for (int chan = 0; chan < resynthBank.CHANNELS; chan++) {
	//	for (int i = 0; i < resynthBank.SAMPLES; i++) {
	//		result[i] += resynthBank[chan][i];
	//	}
	//}



	//FILE* results = fopen("Result.4.wav", "wb");
	//fwrite(result.signal, sizeof(double), result.SAMPLES, results);

	//SignalGrid mixedGrid2 = SignalGrid(*mixedBank, .020*sampleRate, .010*sampleRate);
	//Signal* resynthesized2 = mixedGrid2.resynthesize(*idealBinaryMask2);

	//FILE* results2 = fopen("Result2.wav", "wb");
	//fwrite(resynthesized2->signal, sizeof(double), resynthesized2->SAMPLES, results2);

}

