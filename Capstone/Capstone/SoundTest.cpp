#include "stdafx.h"
#include "SoundTest.h"
#include "Correlogram.h"
#include "LEGION.h"
#include "GroupingNetwork.h"
#include "Cochleagram.h"
#include "IdealBinaryMask.h"

SoundTest::SoundTest(std::string soundDir, std::string resultDir) : soundDir(soundDir), resultDir(resultDir)
{
	soundNames = staticTools::get_all_files_names_within_folder(soundDir);
}

void SoundTest::run() {
	std::fstream resultStream("soundTestResults.txt", std::ios::out);
	resultStream << "\tForeground Energy\tForeground Noise\tBackground Energy\tBackground Noise" endl;

	int sounds = soundNames.size();
	int test = 0;
	int tests = (((float)sounds - 1) / 2) * sounds;
	for (int one = 0; one < sounds; one++) {
		for (int two = one + 1; two < sounds; two++) {
			//print((float)test / tests) endl;
			float percentDone = ((int)round((((float)test / tests) * 10000))) / 100;
			print "\t\t\tDone: " << percentDone << "%" endl;
			std::string name = resultDir + "/" + soundNames[one] + " + " + soundNames[two];
			print name endl;
			//if (name == "results/high c.wav + Tone low.wav") {
				std::string full1 = soundDir + "/" + soundNames[one];
				std::string full2 = soundDir + "/" + soundNames[two];

				Signal* signal1 = staticTools::readWav(full1);
				Signal* signal2 = staticTools::readWav(full2);
				signal1->trim(44100);
				signal2->trim(44100);
				//print full2 endl;

				Signal* mixed = staticTools::combine(*signal1, *signal2);
				
				////delete signal1;
				////delete signal2;
				//mixed->normalize();
				//FILE* mixF = fopen(std::string(name + " (mixed).wav").c_str(), "wb");
				//fwrite(mixed->signal, sizeof(double), mixed->SAMPLES, mixF);
				//fclose(mixF);
				

				Cochleagram displaydsfa(*mixed, mixed->SAMPLE_RATE);
				Correlogram coresdfa(*displaydsfa.cochleagram, 20, 10);
				boolGrid* activeMask = coresdfa.toBinaryMask();
				activeMask->toFile(name + "_Active.txt");
				
				LEGION* lo = new LEGION(coresdfa);
				lo->run();
				lo->markLargestSegment();

				lo->saveSegmentGrid(name + "segmentGrid.txt");
				/*for (int i = 0; i < lo->numSegments; i++) {
					boolGrid* mask = lo->segmentsAsMask(i);
					IdealBinaryMask::saveIdealBinaryMask(std::to_string(i) + "_mask.wav", mask, mixed);
				}*/


				//IdealBinaryMask::saveIdealBinaryMask("LEGION.wav", lo->segmentsAsMask(), mixed);

				GroupingNetwork* level2 = new GroupingNetwork(*lo);
				level2->run();




				//IdealBinaryMask::saveIdealBinaryMask(name + " (background).wav", level2->background, mixed);
				//IdealBinaryMask::saveIdealBinaryMask(name + " (foreground).wav", level2->foreground, mixed);



				SeparationResult* result = errorResults(name, *signal1, *signal2, mixed, level2->foreground, level2->background);
				resultStream << name << "\t" << result->toString() endl;

				mixed->normalize();
				FILE* mixF = fopen(std::string(name + " (mixed).wav").c_str(), "wb");
				fwrite(mixed->signal, sizeof(double), mixed->SAMPLES, mixF);
				fclose(mixF);

				print "result " << result->mean endl;




				/*print "foreground\t\tbackground" endl;
				print "energy\tnoise\tenergy\tnoise" endl;
				print result->foregroundEnergy << "\t" << result->foregroundNoise << "\t";
				print result->backgroundEnergy << "\t" << result->backgroundNoise << "\t" endl;*/

				delete signal1;
				delete signal2;
				delete mixed;
				delete lo;
				delete level2;
				//delete result;
				print "" endl;
				test++;
			//}
		}
	}
	resultStream.close();
}

double SoundTest::energyLoss(Signal& missing, Signal& ideal) {
	double idealSum = ideal.squareSum();
	double ratio = idealSum == 0 ? 1 : missing.squareSum() / idealSum;
	if (ratio > 1) {
		int a = 5;
	}
	return ratio > 1 ? 1 : ratio;
}

SeparationResult* SoundTest::errorResults(std::string name, Signal& signal1, Signal& signal2, Signal* mixed, boolGrid* foregroundMask, boolGrid* backgroundMask) {
	Signal* mixedResynth = SignalGrid::resynthesize(*mixed, foregroundMask->COLUMNS, foregroundMask->ROWS, mixed->SAMPLES, mixed->SAMPLE_RATE, .020*signal1.SAMPLE_RATE, .010*signal1.SAMPLE_RATE);
	mixedResynth->save(std::string("TestResynth.wav"));
	double max = mixedResynth->getMax();
	double maxAmplitude = 1.0 / max;

	//print maxAmplitude endl;
	delete mixedResynth;

	bool normalize = false;

	Signal* foreground = IdealBinaryMask::applyIdealBinaryMask(foregroundMask, mixed);
	Signal* background = IdealBinaryMask::applyIdealBinaryMask(backgroundMask, mixed);
	//foreground->normalize(maxAmplitude);
	//background->normalize(maxAmplitude);
	foreground->save(name + " (foreground).wav");
	background->save(name + " (background).wav");

	
	IdealBinaryMask mask(signal1, signal2);
	//std::fstream of1("ibm1.txt", std::ios::out);
	//mask.writeBinaryMask(of1, mask.idealBinaryMask1);
	//std::fstream of2("ibm2.txt", std::ios::out);
	//mask.writeBinaryMask(of2, mask.idealBinaryMask2);
	//mask.saveIdealBinaryMask("testMask2.wav", mask.idealBinaryMask2 , maxAmplitude);
	//mask.saveIdealBinaryMask("testMask1.wav", mask.idealBinaryMask1 , maxAmplitude);

	Signal* mask1 = mask.applyIdealBinaryMask(mask.idealBinaryMask1, normalize);
	mask1->normalize(maxAmplitude);
	mask1->save(std::string("mask1.wav"));
	Signal* mask2 = mask.applyIdealBinaryMask(mask.idealBinaryMask2, normalize);
	mask2->normalize(maxAmplitude);
	mask2->save(std::string("mask2.wav"));

	boolGrid* Mask1IntersectForeground = mask.idealBinaryMask1->intersect(*foregroundMask);
	boolGrid* Mask2IntersectForeground = mask.idealBinaryMask2->intersect(*foregroundMask);
	Signal* foreground1 = IdealBinaryMask::applyIdealBinaryMask(Mask1IntersectForeground, mixed, normalize);
	foreground1->normalize(maxAmplitude);
	Signal* foreground2 = IdealBinaryMask::applyIdealBinaryMask(Mask2IntersectForeground, mixed, normalize);
	foreground2->normalize(maxAmplitude);

	boolGrid* Mask1IntersectBackground = mask.idealBinaryMask1->intersect(*backgroundMask);
	boolGrid* Mask2IntersectBackground = mask.idealBinaryMask2->intersect(*backgroundMask);
	Signal* background1 = IdealBinaryMask::applyIdealBinaryMask(Mask1IntersectBackground, mixed, normalize);
	background1->normalize(maxAmplitude);
	Signal* background2 = IdealBinaryMask::applyIdealBinaryMask(Mask2IntersectBackground, mixed, normalize);
	background2->normalize(maxAmplitude);

	double energyFore1 = energyLoss(*foreground1, *mask1);
	double energyFore2 = energyLoss(*foreground2, *mask2);

	double energyBack1 = energyLoss(*background1, *mask1);
	double energyBack2 = energyLoss(*background2, *mask2);

	double accuracyOne = ((energyFore1 - energyFore2) + (energyBack2 - energyBack1)) / 2;
	double accuracyTwo = ((energyFore2 - energyFore1) + (energyBack1 - energyBack2)) / 2;
	//print "ErrorOne: " << errorOne endl;
	//print "errorTwo: " << errorTwo endl;

	delete foreground;
	delete background;

	SeparationResult* result = new SeparationResult();
	double error; 
	if (accuracyOne > accuracyTwo) {
		result->mean = accuracyOne;
		result->backgroundEnergy = energyBack2;
		result->backgroundNoise = energyBack1;
		result->foregroundEnergy = energyFore1;
		result->foregroundNoise = energyFore2;
	}
	else {
		result->mean = accuracyTwo;
		result->backgroundEnergy = energyBack1;
		result->backgroundNoise = energyBack2;
		result->foregroundEnergy = energyFore2;
		result->foregroundNoise = energyFore1;
	}

	return result;
}


SoundTest::~SoundTest()
{
}
