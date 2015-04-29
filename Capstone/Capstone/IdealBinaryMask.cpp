#include "stdafx.h"
#include "IdealBinaryMask.h"


IdealBinaryMask::IdealBinaryMask(Signal& signal1, Signal& signal2) : signal1(signal1), signal2(signal2)
{
	int shorter = MIN(signal2.SAMPLES, signal1.SAMPLES);
	signal1.trim(shorter);
	signal2.trim(shorter);

	Cochleagram* coch1 = new Cochleagram(signal1, signal1.SAMPLE_RATE);
	SignalGrid* grid1 = new SignalGrid(*(coch1->cochleagram), .020*signal1.SAMPLE_RATE, .010*signal1.SAMPLE_RATE);
	delete coch1;
	doubleGrid* powerGrid = grid1->toSmrPower();
	delete grid1;

	Cochleagram* coch2 = new Cochleagram(signal2, signal2.SAMPLE_RATE);
	SignalGrid* grid2 = new SignalGrid(*(coch2->cochleagram), .020*signal2.SAMPLE_RATE, .010*signal2.SAMPLE_RATE);
	delete coch2;
	doubleGrid* powerGrid2 = grid2->toSmrPower();
	delete grid2;



	for (int i = 0; i < powerGrid2->ROWS; i++) {
		print minInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) << '\t' << maxInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) << '\t' << avgInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) end;
		print minInArray((*powerGrid).grid[i], powerGrid->COLUMNS) << '\t' << maxInArray((*powerGrid).grid[i], powerGrid->COLUMNS) << '\t' << avgInArray((*powerGrid).grid[i], powerGrid->COLUMNS) end;
	}


	////print powerGrid->COLUMNS << '\t' << powerGrid->ROWS end;
	////print powerGrid2->COLUMNS << '\t' << powerGrid2->ROWS end;


	doubleGrid* decibelGrid = gridOfRelativeDecibels(*powerGrid, *powerGrid2); //simpleRatio(*powerGrid, *powerGrid2);//diffGrid(*powerGrid, *powerGrid2);//gridOfRelativeDecibels(*powerGrid, *powerGrid2);
	doubleGrid* decibelGrid2 = gridOfRelativeDecibels(*powerGrid2, *powerGrid);
	delete powerGrid;
	delete powerGrid2;

	for (int i = 0; i < decibelGrid->ROWS; i++) {
		print minInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << maxInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << avgInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) end;
		print minInArray((*decibelGrid2).grid[i], decibelGrid2->COLUMNS) << '\t' << maxInArray((*decibelGrid2).grid[i], decibelGrid2->COLUMNS) << '\t' << avgInArray((*decibelGrid2).grid[i], decibelGrid2->COLUMNS) end;
	}


	idealBinaryMask1 = createIdealBinaryMask(*decibelGrid, .01);
	idealBinaryMask2 = createIdealBinaryMask(*decibelGrid2, .01);
	delete decibelGrid;
	delete decibelGrid2;
	/*for (int i = 0; i < decibelGrid->ROWS; i++) {
		print minInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << maxInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << avgInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) end;
	}*/




}

Signal* IdealBinaryMask::applyIdealBinaryMask(boolGrid* mask) {
	Signal* mixed = staticTools::combine(signal1, signal2);
	mixed->normalize();

	FilterBank bank(mask->ROWS, 100, 8000, 44100);
	SignalBank* mixedBank = bank.filter(*mixed);
	delete mixed;


	for (int i = 0; i < mask->ROWS; i++) {
		(*mixedBank)[i].reverse();
		(*mixedBank).add((*(bank.bank[i])).filter((*mixedBank)[i]), i);
		(*mixedBank)[i].reverse();
	}

	SignalGrid* grid = new SignalGrid((*mixedBank), .020*mixedBank->SAMPLE_RATE, .010*mixedBank->SAMPLE_RATE);
	Signal* result = grid->resynthesize(*mask);
	delete grid;
	return result;
}

void IdealBinaryMask::saveIdealBinaryMask(std::string name, boolGrid* mask) {
	Signal* result = applyIdealBinaryMask(mask);
	result->normalize();
	FILE* results = fopen(name.c_str(), "wb");
	fwrite(result->signal, sizeof(double), result->SAMPLES, results);
	fclose(results);
	delete result;
}


doubleGrid* IdealBinaryMask::diffGrid(doubleGrid& powerGrid1, doubleGrid& powerGrid2) {
	int combinedRows = MIN(powerGrid1.ROWS, powerGrid2.ROWS);
	int combinedCols = MIN(powerGrid1.COLUMNS, powerGrid2.COLUMNS);

	double** decibelGrid = new double*[combinedRows];
	for (int frame = 0; frame < combinedRows; frame++) {
		decibelGrid[frame] = new double[combinedCols];
		for (int channel = 0; channel < combinedCols; channel++) {
			double one = powerGrid1(frame, channel);
			double two = powerGrid2(frame, channel);
			double diff = one - two;
			decibelGrid[frame][channel] = diff;
		}
	}
	return new doubleGrid(decibelGrid, combinedRows, combinedCols);
}

doubleGrid* IdealBinaryMask::simpleRatio(doubleGrid& powerGrid1, doubleGrid& powerGrid2) {
	int combinedRows = MIN(powerGrid1.ROWS, powerGrid2.ROWS);
	int combinedCols = MIN(powerGrid1.COLUMNS, powerGrid2.COLUMNS);

	double** decibelGrid = new double*[combinedRows];
	for (int frame = 0; frame < combinedRows; frame++) {
		decibelGrid[frame] = new double[combinedCols];
		for (int channel = 0; channel < combinedCols; channel++) {
			double one = powerGrid1(frame, channel);
			double two = powerGrid2(frame, channel);
			decibelGrid[frame][channel] = sqrt(two < .001 ? 200 : one / two);
		}
	}
	return new doubleGrid(decibelGrid, combinedRows, combinedCols);
}

doubleGrid* IdealBinaryMask::gridOfRelativeDecibels(doubleGrid& powerGrid1, doubleGrid& powerGrid2) {
	int combinedRows = MIN(powerGrid1.ROWS, powerGrid2.ROWS);
	int combinedCols = MIN(powerGrid1.COLUMNS, powerGrid2.COLUMNS);

	double** decibelGrid = new double*[combinedRows];
	for (int channel = 0; channel < combinedRows; channel++) {
		decibelGrid[channel] = new double[combinedCols];
		for (int frame = 0; frame < combinedCols; frame++) {
			double one = powerGrid1(channel, frame);
			double two = powerGrid2(channel, frame);
			if (one == 0 || two == 0) {
				decibelGrid[channel][frame] = one > two ? 1000 : -1000;
			}
			else {
				double decibel = 20 * log10(
					one / two
					);
				decibelGrid[channel][frame] = decibel;
			}
		}
	}
	return new doubleGrid(decibelGrid, combinedRows, combinedCols);
}

doubleGrid* IdealBinaryMask::gridOfRelativeDecibelRatios(doubleGrid& powerGrid1, doubleGrid& powerGrid2) {
	int combinedRows = MIN(powerGrid1.ROWS, powerGrid2.ROWS);
	int combinedCols = MIN(powerGrid1.COLUMNS, powerGrid2.COLUMNS);

	double** decibelGrid = new double*[combinedRows];
	for (int frame = 0; frame < combinedRows; frame++) {
		decibelGrid[frame] = new double[combinedCols];
		for (int channel = 0; channel < combinedCols; channel++) {
			double one = powerGrid1(frame, channel);
			double two = powerGrid2(frame, channel);
			if (one == 0 || two == 0) {
				decibelGrid[frame][channel] = one > two ? 1000 : -1000;
			}
			else {
				double decibel1 = 20 * log10(one);
				double decibel2 = 20 * log10(two);
				decibelGrid[frame][channel] = decibel1 / decibel2;
			}
		}
	}
	return new doubleGrid(decibelGrid, combinedRows, combinedCols);
}

boolGrid* IdealBinaryMask::createIdealBinaryMask(doubleGrid& decibelGrid, double decibelReq) {
	bool** binaryMask = new bool*[decibelGrid.ROWS];
	for (int row = 0; row < decibelGrid.ROWS; row++) {
		binaryMask[row] = new bool[decibelGrid.COLUMNS];
		for (int col = 0; col < decibelGrid.COLUMNS; col++) {
			binaryMask[row][col] = decibelGrid(row, col) >= decibelReq;
		}
	}
	return new boolGrid(binaryMask, decibelGrid.ROWS, decibelGrid.COLUMNS);
}


IdealBinaryMask::~IdealBinaryMask()
{
}
