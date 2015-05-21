#include "stdafx.h"
#include "IdealBinaryMask.h"


IdealBinaryMask::IdealBinaryMask(Signal& signal1, Signal& signal2) : signal1(signal1), signal2(signal2)
{
	int shorter = MIN(signal2.SAMPLES, signal1.SAMPLES);
	signal1.trim(shorter);
	signal2.trim(shorter);

	//Cochleagram* coch1 = new Cochleagram(signal1, signal1.SAMPLE_RATE);

	FilterBank* bank1 = new FilterBank(Cochleagram::CHANNELS, Cochleagram::MIN_FREQ, Cochleagram::MAX_FREQ, signal1.SAMPLE_RATE);
	SignalBank* freqBank1 = bank1->filter(signal1);
	SignalGrid* grid1 = new SignalGrid(*freqBank1, .020*signal1.SAMPLE_RATE, .010*signal1.SAMPLE_RATE);
	delete bank1;
	delete freqBank1;
	doubleGrid* powerGrid = grid1->toSmrPower();
	delete grid1;

	//Cochleagram* coch2 = new Cochleagram(signal2, signal2.SAMPLE_RATE);
	FilterBank* bank2 = new FilterBank(Cochleagram::CHANNELS, Cochleagram::MIN_FREQ, Cochleagram::MAX_FREQ, signal1.SAMPLE_RATE);
	SignalBank* freqBank2 = bank2->filter(signal2);
	SignalGrid* grid2 = new SignalGrid(*freqBank2, .020*signal2.SAMPLE_RATE, .010*signal2.SAMPLE_RATE);
	delete bank2;
	delete freqBank2;
	doubleGrid* powerGrid2 = grid2->toSmrPower();
	delete grid2;



	//for (int i = 0; i < powerGrid2->ROWS; i++) {
	//	print minInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) << '\t' << maxInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) << '\t' << avgInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) endl;
	//	print minInArray((*powerGrid).grid[i], powerGrid->COLUMNS) << '\t' << maxInArray((*powerGrid).grid[i], powerGrid->COLUMNS) << '\t' << avgInArray((*powerGrid).grid[i], powerGrid->COLUMNS) endl;
	//}


	////print powerGrid->COLUMNS << '\t' << powerGrid->ROWS endl;
	////print powerGrid2->COLUMNS << '\t' << powerGrid2->ROWS endl;


	doubleGrid* decibelGrid = gridOfRelativeDecibels(*powerGrid, *powerGrid2); //simpleRatio(*powerGrid, *powerGrid2);//diffGrid(*powerGrid, *powerGrid2);//gridOfRelativeDecibels(*powerGrid, *powerGrid2);
	doubleGrid* decibelGrid2 = gridOfRelativeDecibels(*powerGrid2, *powerGrid);
	delete powerGrid;
	delete powerGrid2;

	//for (int i = 0; i < decibelGrid->ROWS; i++) {
	//	print minInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << maxInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << avgInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) endl;
	//	print minInArray((*decibelGrid2).grid[i], decibelGrid2->COLUMNS) << '\t' << maxInArray((*decibelGrid2).grid[i], decibelGrid2->COLUMNS) << '\t' << avgInArray((*decibelGrid2).grid[i], decibelGrid2->COLUMNS) endl;
	//}


	idealBinaryMask1 = createIdealBinaryMask(*decibelGrid, 0);// .01);
	idealBinaryMask2 = createIdealBinaryMask(*decibelGrid2, 0);// .01);
	delete decibelGrid;
	delete decibelGrid2;
	/*for (int i = 0; i < decibelGrid->ROWS; i++) {
		print minInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << maxInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << avgInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) endl;
	}*/




}

Signal* IdealBinaryMask::applyIdealBinaryMask(boolGrid* mask, bool normalize) {
	Signal* mixed = staticTools::combine(signal1, signal2);
	//if (normalize)
		mixed->normalize();

	Signal* result = applyIdealBinaryMask(mask, mixed, normalize);
	delete mixed;
	return result;
}

Signal* IdealBinaryMask::applyIdealBinaryMask(boolGrid* mask, Signal* signal, bool normalize) {

	FilterBank bank(mask->ROWS, Cochleagram::MIN_FREQ, Cochleagram::MAX_FREQ, 44100);
	SignalBank* mixedBank = bank.filter(*signal, false);


	for (int i = 0; i < mask->ROWS; i++) {
		(*mixedBank)[i].reverse();

		(*mixedBank).add((*(bank.bank[i])).filter((*mixedBank)[i], false), i);
		(*mixedBank)[i].reverse();
	}

	SignalGrid* grid = new SignalGrid((*mixedBank), .020*mixedBank->SAMPLE_RATE, .010*mixedBank->SAMPLE_RATE);
	Signal* result = grid->resynthesize(*mask);
	if (normalize)
		result->normalize();
	delete grid;
	delete mixedBank;
	return result;
}

void IdealBinaryMask::saveIdealBinaryMask(std::string name, boolGrid* mask, double normal) {
	Signal* result = applyIdealBinaryMask(mask, normal);
	if (normal != 1) {
		result->normalize(normal);
	}
	FILE* results = fopen(name.c_str(), "wb");
	fwrite(result->signal, sizeof(double), result->SAMPLES, results);
	fclose(results);
	delete result;
}

void IdealBinaryMask::saveIdealBinaryMask(std::string name, boolGrid* mask, Signal* signal) {
	Signal* result = applyIdealBinaryMask(mask, signal);

	FILE* results = fopen(name.c_str(), "wb");
	fwrite(result->signal, sizeof(double), result->SAMPLES, results);
	fclose(results);
	delete result;
}

void IdealBinaryMask::writeBinaryMask(std::ostream& os, boolGrid* mask) {
	for (int i = mask->ROWS - 1; i >= 0; --i)
	{
		for (int j = 0; j < mask->COLUMNS; j++)
		{
			int length = std::to_string((*mask)(i, j)).length();
			os << (*mask)(i, j);
			for (int x = 0; x < 2 - length; x++) {
				os << " ";
			}
		}
		os << "\n";
	}
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
