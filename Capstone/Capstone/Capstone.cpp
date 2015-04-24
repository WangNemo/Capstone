// Capstone.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "definitions.h"
#include<fstream>
#include <string>

#include "testing.h"
#include "SignalBank.h"
#include "SignalGrid.h"
#include "GammatoneFilter.h"
#include "Correlogram.h"

doubleGrid* diffGrid(doubleGrid& powerGrid1, doubleGrid& powerGrid2) {
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

doubleGrid* simpleRatio(doubleGrid& powerGrid1, doubleGrid& powerGrid2) {
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

doubleGrid* gridOfRelativeDecibels(doubleGrid& powerGrid1, doubleGrid& powerGrid2) {
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
				double decibel = 20 * log10(
					one / two
					);
				decibelGrid[frame][channel] = decibel;
			}
		}
	}
	return new doubleGrid(decibelGrid, combinedRows, combinedCols);
}

doubleGrid* gridOfRelativeDecibelRatios(doubleGrid& powerGrid1, doubleGrid& powerGrid2) {
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

boolGrid* createIdealBinaryMask(doubleGrid& decibelGrid, double decibelReq) {
	bool** binaryMask = new bool*[decibelGrid.ROWS];
	for (int row = 0; row < decibelGrid.ROWS; row++) {
		binaryMask[row] = new bool[decibelGrid.COLUMNS];
		for (int col = 0; col < decibelGrid.COLUMNS; col++) {
			binaryMask[row][col] = decibelGrid(row, col) >= decibelReq;
		}
	}
	return new boolGrid(binaryMask, decibelGrid.ROWS, decibelGrid.COLUMNS);
}

int main(int argc, char* argv[], char* envp[]) {
	print argv[0] end;
	print argv[1] end;
	print argv[2] end;
	std::string fileName(argv[1]);
	std::string fileName2(argv[2]);
	Signal* signal1 = staticTools::readWav(fileName);



	Signal* signal2 = staticTools::readWav(fileName2);
	int shorter = MIN(signal2->SAMPLES, signal1->SAMPLES);
	signal1->trim(shorter);
	signal2->trim(shorter);

	int sampleRate = 44100;
	int channels = 32;


	Signal* mixed = staticTools::combine(*signal1, *signal2);
	mixed->normalize();
	FILE* mixF = fopen("Mixed.wav", "wb");
	fwrite(mixed->signal, sizeof(double), mixed->SAMPLES, mixF);

	Cochleagram* cochMix = new Cochleagram(*mixed, sampleRate);
	Correlogram* correl = new Correlogram(*cochMix->cochleagram, 20, 10);

	
	//return 0;

	////Signal* corr = mixed->autoCorrelate(100);
	////corr->normalize();
	////FILE* corrTest = fopen("corrTest.wav", "wb");
	////fwrite(corr->signal, sizeof(double), corr->SAMPLES, corrTest);

	//return 0;

	//Cochleagram* coch1 = new Cochleagram(*signal1, sampleRate);

	//delete signal1;
	//SignalGrid* grid1 = new SignalGrid(*(coch1->cochleagram), .020*sampleRate, .010*sampleRate);
	//print grid1->CHANNELS << '\t' << grid1->FRAMES end;
	//delete coch1;
	//doubleGrid* powerGrid = grid1->toSmrPower();
	//delete grid1;


	//Cochleagram* coch2 = new Cochleagram(*signal2, sampleRate);
	//delete signal2;
	//SignalGrid* grid2 = new SignalGrid(*(coch2->cochleagram), .020*sampleRate, .010*sampleRate);
	//print grid2->CHANNELS << '\t' << grid2->FRAMES end;
	//delete coch2;
	//doubleGrid* powerGrid2 = grid2->toSmrPower();
	//delete grid2;

	////for (int i = 0; i < powerGrid2->ROWS; i++) {
	////	print minInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) << '\t' << maxInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) << '\t' << avgInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) end;
	////	print minInArray((*powerGrid).grid[i], powerGrid->COLUMNS) << '\t' << maxInArray((*powerGrid).grid[i], powerGrid->COLUMNS) << '\t' << avgInArray((*powerGrid).grid[i], powerGrid->COLUMNS) end;
	////}


	////print powerGrid->COLUMNS << '\t' << powerGrid->ROWS end;
	////print powerGrid2->COLUMNS << '\t' << powerGrid2->ROWS end;


	//doubleGrid* decibelGrid = gridOfRelativeDecibels(*powerGrid, *powerGrid2); //simpleRatio(*powerGrid, *powerGrid2);//diffGrid(*powerGrid, *powerGrid2);//gridOfRelativeDecibels(*powerGrid, *powerGrid2);
	//doubleGrid* decibelGrid2 = gridOfRelativeDecibels(*powerGrid2, *powerGrid);
	//delete powerGrid;
	//delete powerGrid2;


	//boolGrid* idealBinaryMask1 = createIdealBinaryMask(*decibelGrid, 0);
	//boolGrid* idealBinaryMask2 = createIdealBinaryMask(*decibelGrid2, 0);

	///*for (int i = 0; i < decibelGrid->ROWS; i++) {
	//	print minInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << maxInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << avgInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) end;
	//}*/


	bool** binaryMask = new bool*[correl->T_FGrid->FRAMES];
	for (int row = 0; row < correl->T_FGrid->FRAMES; row++) {
		binaryMask[row] = new bool[correl->T_FGrid->CHANNELS];
		for (int col = 0; col < correl->T_FGrid->CHANNELS; col++) {
			binaryMask[row][col] = (*correl->T_FGrid)[row][col][0] > 0;
		}
	}
	boolGrid* idealBinaryMask1 = new boolGrid(binaryMask, correl->T_FGrid->FRAMES, correl->T_FGrid->CHANNELS);


	FilterBank bank(channels, 100, 8000, 44100);
	SignalBank* mixedBank = bank.filter(*mixed);

	for (int i = 0; i < channels; i++) {
		(*mixedBank)[i].reverse();
		(*mixedBank).add((*(bank.bank[i])).filter((*mixedBank)[i]), i);
		(*mixedBank)[i].reverse();
	}


	
	SignalGrid mixedGrid = SignalGrid(*mixedBank, .020*sampleRate, .010*sampleRate);
	Signal* resynthesized = mixedGrid.resynthesize(*idealBinaryMask1);

	FILE* results = fopen("Result1.wav", "wb");
	fwrite(resynthesized->signal, sizeof(double), resynthesized->SAMPLES, results);


	//SignalGrid mixedGrid2 = SignalGrid(*mixedBank, .020*sampleRate, .010*sampleRate);
	//Signal* resynthesized2 = mixedGrid2.resynthesize(*idealBinaryMask2);

	//FILE* results2 = fopen("Result2.wav", "wb");
	//fwrite(resynthesized2->signal, sizeof(double), resynthesized2->SAMPLES, results2);

}

