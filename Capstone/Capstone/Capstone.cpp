// Capstone.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "definitions.h"

#include "testing.h"
#include "SignalBank.h"
#include "SignalGrid.h"
#include "GammatoneFilter.h"
#include "Correlogram.h"
#include "crossCorrelationSegmentation.h"
#include "IdealBinaryMask.h"
#include <ctime>
#include "LEGION.h"
#include "GroupingNetwork.h"
#include <vector>
#include "SoundTest.h"
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

struct AtExit
{
	~AtExit() { _CrtDumpMemoryLeaks(); }
} doAtExit;

int main(int argc, char* argv[], char* envp[]) {
	srand(static_cast <unsigned> (time(0)));

	SoundTest test("sounds", "results");
	test.run();
	/*for each (std::string var in staticTools::get_all_files_names_within_folder(L"sounds"))
	{
		print var endl;
		Signal* signal1 = staticTools::readWav(var);
	}*/
	_CrtDumpMemoryLeaks();
	return 0;

	//print argv[0] endl;
	//print argv[1] endl;
	//print argv[2] endl;



	std::string fileName(argv[1]);
	std::string fileName2(argv[2]);
	Signal* signal1 = staticTools::readWav(fileName);


	Signal* signal2 = staticTools::readWav(fileName2);
	int shorter = MIN(signal2->SAMPLES, signal1->SAMPLES);
	signal1->trim(shorter);
	signal2->trim(shorter);



	return 0;

	Signal* mixed = staticTools::combine(*signal1, *signal2);
	mixed->normalize();
	mixed->trim(44100);
	FILE* mixF = fopen("Mixed.wav", "wb");
	fwrite(mixed->signal, sizeof(double), mixed->SAMPLES, mixF);

	//double total = 0;
	//double low = 10;
	//double max = -5;
	//for (int i = 0; i < 10000000; i++) {
	//	double rand = staticTools::nonZeroGaussianRandom(0);
	//	total += rand;
	//	if (rand < low) low = rand;
	//	if (rand > max) max = rand;
	//}


	Cochleagram displaydsfa(*mixed, mixed->SAMPLE_RATE);
	Correlogram coresdfa(*displaydsfa.cochleagram, 20, 10);
	//std::fstream ofsas("silentMask.txt", std::ios::out);
	//boolGrid* mask41 = coresdfa.toBinaryMask();
	//IdealBinaryMask::writeBinaryMask(ofsas, mask41);

	LEGION* lo = new LEGION(coresdfa);
	lo->run();
	lo->markLargestSegment();
	//IdealBinaryMask::saveIdealBinaryMask("LEGION.wav", lo->segmentsAsMask(), mixed);

	GroupingNetwork* level2 = new GroupingNetwork(*lo);
	level2->run();

	IdealBinaryMask::saveIdealBinaryMask("background.wav", level2->background, mixed);
	IdealBinaryMask::saveIdealBinaryMask("foreground.wav", level2->foreground, mixed);

	return 0;
	//for (int i = 7; i >= 0; i--) {
	//	FILE* mixF = fopen(("corr" + std::to_string(8 - i)+  ".wav").c_str(), "wb");
	//	fwrite((*coresdfa.T_FGrid)[97][i].signal, sizeof(double), (*coresdfa.T_FGrid)[97][i].SAMPLES, mixF);
	//}


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
	print correl->T_FGrid->FRAMES endl;
	crossCorrelationSegmentation segmentation(*correl->T_FGrid);
	
	//boolGrid* idealBinaryMask = correl->toBinaryMask();
	//return 0;

	////Signal* corr = mixed->autoCorrelate(100);
	////corr->normalize();
	////FILE* corrTest = fopen("corrTest.wav", "wb");
	////fwrite(corr->signal, sizeof(double), corr->SAMPLES, corrTest);

	//return 0;

	//Cochleagram* cochMix = new Cochleagram(*mixed, signal1.SAMPLE_RATE);




	segmentation.writeSegmentText("Map.txt");

	//print "groups: " << segmentation.groups endl;
	////SignalBank resynthBank(segmentation.groups - 1, mixedBank->SAMPLE_RATE, mixedBank->SAMPLES);
	//for (int group = 1; group < segmentation.groups - 1; group++) {
	//	print group endl;
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
	//	print group endl;
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

