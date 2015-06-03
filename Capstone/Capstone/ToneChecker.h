#pragma once
#include "Definitions.h"
#include "Correlogram.h"
#include "crossCorrelationSegmentation.h"

class ToneChecker
{
private:
	int toneWidthSteps = 6;
	double minConnection = .8;
	double maxConnection = 1;
	double stepAmount = .0001;
	double toneWidth = toneWidthSteps * stepAmount;


	/*double lowToneMin = .9911;
	double lowToneMax = .9954;
	double midToneMin = .9725;
	double midToneMax = .9775;
	double highToneMin = .876;
	double highToneMax = .8815;
	double minChangeReq = 950;*/
	bool toneFound = false;
	double specialMin, specialMax, largest = 0;
	Correlogram& correlogram;
	doubleGrid* connectionGrid;
	crossCorrelationSegmentation* specialSegmenter;

	int sumArray(int* changeArray);
public:
	ToneChecker(Correlogram& correlogram);
	int changeInRange(double min, double max);
	boolGrid* changeInRangeGrid(double min, double max);
	bool foundTone();
	//bool isLow();
	//bool isMid();
	//bool isHigh();
	void searchForTone();
	boolGrid* getForeground();
	boolGrid* getBackground();
	~ToneChecker();
};
