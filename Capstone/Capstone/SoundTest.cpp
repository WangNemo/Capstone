#include "stdafx.h"
#include "SoundTest.h"
#include "Correlogram.h"
#include "LEGION.h"
#include "GroupingNetwork.h"
#include "Cochleagram.h"
#include "IdealBinaryMask.h"
#include "ToneChecker.h"

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
				mixed->normalize();

				////delete signal1;
				////delete signal2;
				//mixed->normalize();
				//FILE* mixF = fopen(std::string(name + " (mixed).wav").c_str(), "wb");
				//fwrite(mixed->signal, sizeof(double), mixed->SAMPLES, mixF);
				//fclose(mixF);
				

				Cochleagram displaydsfa(*mixed, mixed->SAMPLE_RATE);
				Correlogram coresdfa(*displaydsfa.cochleagram, 20, 10);
				ToneChecker tc(coresdfa);

				boolGrid* foreground;
				boolGrid* background;
				if (tc.isSpecial()) {
					print "TONE FOUND" endl;
					foreground = tc.getForeground();
					background = tc.getBackground();
				}
				else {

					boolGrid* activeMask = coresdfa.toBinaryMask();
					activeMask->toFile(name + "_Active.txt");

					LEGION* lo = new LEGION(coresdfa, name);
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

					foreground = level2->foreground;
					background = level2->background;

					delete lo;
					delete level2;
				}



				//IdealBinaryMask::saveIdealBinaryMask(name + " (background).wav", level2->background, mixed);
				//IdealBinaryMask::saveIdealBinaryMask(name + " (foreground).wav", level2->foreground, mixed);



			SeparationResult* result = errorResults(name, *signal1, *signal2, mixed, foreground, background);
			resultStream << name << "\t" << result->toString() endl;

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
			delete result;
			delete foreground;
			delete background;
			print "" endl;
			test++;
			//}
		}
	}
	resultStream.close();
}

double SoundTest::energyLoss(Signal& missing, Signal& ideal) {
	double idealSum = ideal.squareSum();
	double present = missing.squareSum();
	double ratio = idealSum == 0 ? 1 :  present / idealSum;
	print ratio endl;
	if (abs(ratio - .41258) < .001) {
		print "\t" << present << "\t" << idealSum endl;
		print "\t" << missing.SAMPLES << "\t" << ideal.SAMPLES endl;
		print "\t" << missing.getMean() << "\t" << ideal.getMean() endl;
	}
	return ratio > 1 ? 1 : ratio;
}

SeparationResult* SoundTest::errorResults(std::string name, Signal& signal1, Signal& signal2, Signal* mixed, boolGrid* foregroundMask, boolGrid* backgroundMask) {
	//Signal* mixedResynth = SignalGrid::resynthesize(*mixed, foregroundMask->COLUMNS, foregroundMask->ROWS, mixed->SAMPLES, mixed->SAMPLE_RATE, .020*signal1.SAMPLE_RATE, .010*signal1.SAMPLE_RATE);
	//mixedResynth->save(std::string("TestResynth.wav"));
	//double max = mixedResynth->getMax();
	//print max endl;
	//double maxAmplitude = max;// 1.0 / max;

	//print maxAmplitude endl;
	//delete mixedResynth;

	bool normalize = false;

	foregroundMask->toFile(name + " foregroundMask.txt");
	backgroundMask->toFile(name + " backgroundMask.txt");

	Signal* foreground = IdealBinaryMask::applyIdealBinaryMask(foregroundMask, mixed);
	Signal* background = IdealBinaryMask::applyIdealBinaryMask(backgroundMask, mixed);
	//foreground->normalize(maxAmplitude);
	//background->normalize(maxAmplitude);
	foreground->save(name + " (foreground).wav");
	background->save(name + " (background).wav");

	
	IdealBinaryMask mask(signal1, signal2);


	std::fstream of1(name + " ibm1.txt", std::ios::out);
	mask.writeBinaryMask(of1, mask.idealBinaryMask1);
	std::fstream of2(name + " ibm2.txt", std::ios::out);
	mask.writeBinaryMask(of2, mask.idealBinaryMask2);
	mask.saveIdealBinaryMask(name + " ibm1.wav", mask.idealBinaryMask2);
	mask.saveIdealBinaryMask(name + " ibm2.wav", mask.idealBinaryMask1);

	Signal* mask1 = mask.applyIdealBinaryMask(mask.idealBinaryMask1, normalize);
	mask1->save(std::string("mask1.wav"));
	Signal* mask2 = mask.applyIdealBinaryMask(mask.idealBinaryMask2, normalize);
	mask2->save(std::string("mask2.wav"));

	boolGrid* Mask1IntersectForeground = mask.idealBinaryMask1->intersect(*foregroundMask);

	boolGrid* Mask2IntersectForeground = mask.idealBinaryMask2->intersect(*foregroundMask);
	Signal* foreground1 = IdealBinaryMask::applyIdealBinaryMask(Mask1IntersectForeground, mixed, normalize);
	Signal* foreground2 = IdealBinaryMask::applyIdealBinaryMask(Mask2IntersectForeground, mixed, normalize);

	boolGrid* Mask1IntersectBackground = mask.idealBinaryMask1->intersect(*backgroundMask);
	boolGrid* Mask2IntersectBackground = mask.idealBinaryMask2->intersect(*backgroundMask);
	Signal* background1 = IdealBinaryMask::applyIdealBinaryMask(Mask1IntersectBackground, mixed, normalize);
	Signal* background2 = IdealBinaryMask::applyIdealBinaryMask(Mask2IntersectBackground, mixed, normalize);

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
