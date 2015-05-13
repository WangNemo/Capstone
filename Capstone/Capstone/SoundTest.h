#pragma once
#include "Definitions.h"

class SoundTest
{
private: 
	std::string soundDir;
	std::string resultDir;
	std::vector <std::string> soundNames;
public:
	SoundTest(std::string soundDir, std::string resultDir);
	void run();
	double energyLoss(Signal& separated, Signal& ideal);
	SeparationResult* errorResults(std::string name, Signal& signal1, Signal& signal2, Signal* mixed, boolGrid* foregroundMask, boolGrid* backgroundMask);
	~SoundTest();
};

