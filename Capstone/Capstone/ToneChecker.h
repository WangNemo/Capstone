#pragma once
#include "Definitions.h"
#include "Correlogram.h"
#include "crossCorrelationSegmentation.h"

class ToneChecker
{
private:
	double lowToneMin = .9911;
	double lowToneMax = .9954;
	double midToneMin = .9725;
	double midToneMax = .9775;
	double highToneMin = .876;
	double highToneMax = .8815;
	double minChangeReq = 950;
	double specialMin, specialMax, largest = 0;
	Correlogram& correlogram;
	doubleGrid* connectionGrid;
	crossCorrelationSegmentation* speicalSegmenter;

public:
	ToneChecker(Correlogram& correlogram);
	int changeInRange(double min, double max);
	boolGrid* changeInRangeGrid(double min, double max);
	bool isSpecial();
	bool isLow();
	bool isMid();
	bool isHigh();
	boolGrid* getForeground();
	boolGrid* getBackground();
	~ToneChecker();
};
