#pragma once
#include "Definitions.h"
#include "Correlogram.h"
#include "crossCorrelationSegmentation.h"

class ToneChecker
{
private:
	double lowToneMin = .992;
	double lowToneMax = .9954;
	double midToneMin = .973;
	double midToneMax = .977;
	double highToneMin = .8767;
	double highToneMax = .8809;
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

