#include "stdafx.h"
#include "ToneChecker.h"


ToneChecker::ToneChecker(Correlogram& correlogram) : correlogram(correlogram)
{
	//doubleGrid* connectionGrid = new doubleGrid(correlogram.T_FGrid->CHANNELS - 1);
	double** grid = new double*[correlogram.T_FGrid->CHANNELS - 1];
	for (int channel = 0; channel < correlogram.T_FGrid->CHANNELS - 1; channel++) {
		grid[channel] = new double[correlogram.T_FGrid->FRAMES];
		for (int frame = 0; frame < correlogram.T_FGrid->FRAMES; frame++) {
			double crossCorrelation = (*correlogram.T_FGrid)[frame][channel].crossCorrelate((*correlogram.T_FGrid)[frame][channel + 1]);
			grid[channel][frame] = crossCorrelation;
		}
	}
	connectionGrid = new doubleGrid(grid, correlogram.T_FGrid->CHANNELS - 1, correlogram.T_FGrid->FRAMES);
	searchForTone();
}

int ToneChecker::changeInRange(double min, double max){
	return connectionGrid->countOver(min) - connectionGrid->countOver(max);
}

boolGrid* ToneChecker::changeInRangeGrid(double min, double max){
	bool** grid = new bool*[correlogram.T_FGrid->CHANNELS];
	for (int channel = 0; channel < correlogram.T_FGrid->CHANNELS - 1; channel++) {
		grid[channel] = new bool[correlogram.T_FGrid->FRAMES];
		for (int frame = 0; frame < correlogram.T_FGrid->FRAMES; frame++) {
			double value = (*connectionGrid)(channel, frame);
			grid[channel][frame] = min < value && value < max;
		}
	}
	grid[correlogram.T_FGrid->CHANNELS - 1] = new bool[correlogram.T_FGrid->FRAMES];
	for (int frame = 0; frame < correlogram.T_FGrid->FRAMES; frame++) {
		grid[correlogram.T_FGrid->CHANNELS - 1][frame] = grid[correlogram.T_FGrid->CHANNELS - 2][frame];
	}

	return new boolGrid(grid, correlogram.T_FGrid->CHANNELS, correlogram.T_FGrid->FRAMES);
}

void ToneChecker::searchForTone() {
	int count = 0;
	int maxWidth = 0;
	int maxSize = 0;
	//crossCorrelationSegmentation* bestSegmenter = nullptr;
	double min = maxConnection;
	double max = minConnection;

	for (double i = maxConnection; i > minConnection + toneWidth; i -= stepAmount) {
		boolGrid* grid = changeInRangeGrid(i - toneWidth, i);
		crossCorrelationSegmentation* segmenter = new crossCorrelationSegmentation(grid);
		int segNum = segmenter->getLargestSegment();
		if (segNum > 0) {
			int segSize = segmenter->segmentSize(segNum);
			int width = segmenter->timeLength(segNum);//changeInRange(i - toneWidth, i);
			if (width > maxWidth || (width == maxWidth && segSize > maxSize)) {
				maxWidth = width;
				maxSize = segSize;
				min = i - toneWidth;
				max = i;
				//if (bestSegmenter != nullptr) {
				//	delete bestSegmenter;
				//}
				//bestSegmenter = segmenter;
			}
		}
		delete segmenter;
	}
	//print "CHANGE: " << maxChange << "\tmin: " << min << "\tmax: " << max endl;
	if (maxWidth > 80 && maxSize > 500)  {
		print "\t\tFOUND " << maxWidth << " " << maxSize << " " << min << " " << max endl;
		specialMin = min;
		specialMax = max;
		boolGrid* grid = changeInRangeGrid(min, max);
		specialSegmenter = new crossCorrelationSegmentation(grid);
		//specialSegmenter = bestSegmenter;
		toneFound = true;
	}
	else {
		print "\t\tnot " << maxWidth << " " << maxSize endl;

	}

	//boolGrid* grid = changeInRangeGrid(min, max);
	//crossCorrelationSegmentation* segmenter = new crossCorrelationSegmentation(*grid);
	//int segNum = segmenter->getLargestSegment();
	//int segSize = segmenter->segmentSize(segNum);
	//print "SEG SIZE: " << segSize << " SEG LENGTH: " << segmenter->timeLength(segNum) endl;
}

int ToneChecker::sumArray(int* changeArray) {
	int sum = 0;
	for (int i = 0; i < toneWidth; i++) {
		sum += changeArray[i];
	}
	return sum;
}


bool ToneChecker::foundTone(){
	//bool low = isLow();
	//bool mid = isMid();
	//bool high = isHigh(); // to prevent short circuit
	///*if (specialMin == lowToneMin) print "LOW" endl;
	//if (specialMin == midToneMin) print "MID" endl;
	//if (specialMin == highToneMin) print "HIGH" endl;*/
	//return low || mid || high;
	return toneFound;
}

//bool ToneChecker::isLow() {
//	int change = changeInRange(lowToneMin, lowToneMax);
//	
//	boolGrid* grid = changeInRangeGrid(lowToneMin, lowToneMax);
//	crossCorrelationSegmentation* segmenter = new crossCorrelationSegmentation(*grid);
//	int segNum = segmenter->getLargestSegment();
//	int segSize = segmenter->segmentSize(segNum);
//	//if (segNum == 0) {
//	//	print "NO SEGMENTS" endl;
//	//}
//
//	bool found = segSize > minChangeReq;
//	if (found && segSize > largest) {
//		largest = segSize;
//		specialMin = lowToneMin;
//		specialMax = lowToneMax;
//		speicalSegmenter = segmenter;
//	}
//	//print "MID " << segSize << "\tCOUNT " << change << "\tsegs " << segmenter.groups endl;
//	return found;
//}
//
//bool ToneChecker::isMid(){
//	int change = changeInRange(midToneMin, midToneMax);
//
//	boolGrid* grid = changeInRangeGrid(midToneMin, midToneMax);
//	crossCorrelationSegmentation* segmenter = new crossCorrelationSegmentation(*grid);
//	int segNum = segmenter->getLargestSegment();
//	int segSize = segmenter->segmentSize(segNum);
//	/*if (segNum == 0) {
//		print "NO SEGMENTS" endl;
//	}*/
//
//	bool found = segSize > minChangeReq;
//	if (found && segSize > largest) {
//		largest = segSize;
//		specialMin = midToneMin;
//		specialMax = midToneMax;
//		speicalSegmenter = segmenter;
//	}
//	//print "MID " << segSize << "\tCOUNT " << change << "\tsegs " << segmenter.groups endl;
//	return found;
//}
//
//bool ToneChecker::isHigh(){
//	int change = changeInRange(highToneMin, highToneMax);
//
//	boolGrid* grid = changeInRangeGrid(highToneMin, highToneMax);
//	crossCorrelationSegmentation* segmenter = new crossCorrelationSegmentation(*grid);
//	int segNum = segmenter->getLargestSegment();
//	int segSize = segmenter->segmentSize(segNum);
//	/*if (segNum == 0) {
//		print "NO SEGMENTS" endl;
//	}*/
//
//	bool found = segSize > minChangeReq;
//	if (found && segSize > largest) {
//		largest = segSize;
//		specialMin = highToneMin;
//		specialMax = highToneMax;
//		speicalSegmenter = segmenter;
//	}
//	//print "MID " << segSize << "\tCOUNT " << change << "\tsegs " << segmenter.groups endl;
//	return found;
//}

boolGrid* ToneChecker::getForeground(){
	return specialSegmenter->getBinaryMask(specialSegmenter->getLargestSegment());
}

boolGrid* ToneChecker::getBackground(){
	boolGrid* background = specialSegmenter->getBinaryMask(specialSegmenter->getLargestSegment());
	background->invert();
	return background;
}


ToneChecker::~ToneChecker()
{
	delete specialSegmenter;
	delete connectionGrid;
}
