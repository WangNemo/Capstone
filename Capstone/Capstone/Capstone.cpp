// Capstone.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "definitions.h"
#include<fstream>

#include "testing.h"
#include "SignalBank.h"
#include "SignalGrid.h"

doubleGrid* gridOfRelativeDecibels(doubleGrid& powerGrid1, doubleGrid& powerGrid2) {
	int combinedRows = MAX(powerGrid1.ROWS, powerGrid2.ROWS);
	int combinedCols = MAX(powerGrid1.COLUMNS, powerGrid2.COLUMNS);

	double** decibelGrid = new double*[combinedRows];
	for (int frame = 0; frame < combinedRows; frame++) {
		decibelGrid[frame] = new double[combinedCols];
		for (int channel = 0; channel < combinedCols; channel++) {
			double one = powerGrid1(frame, channel);
			double two = powerGrid2(frame, channel);
			if (two > 1800 || two < 1750) {
				int a = 0;
			}
			if (one == 0 || two == 0) {
				decibelGrid[frame][channel] = one > two ? 1000 : -1000;
			}
			else {
				double decibel = 20 * log10(
					one / two
					);
				if (decibel > .1) {
					int a = 1;
				}
				if (decibel < -.1) {
					int a = 1;
				}
				decibelGrid[frame][channel] = decibel;
			}
		}
	}
	return new doubleGrid(decibelGrid, combinedRows, combinedCols);
}

boolGrid* createIdealBinaryMask(doubleGrid& decibelGrid, int decibelReq) {
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

	int sampleRate = 44100;
	int channels = 128;

	Cochleagram* coch1 = new Cochleagram(*signal1, sampleRate);
	//delete signal1;
	SignalGrid grid1(*(coch1->cochleagram), .020*sampleRate, .010*sampleRate);
	delete coch1;
	doubleGrid* powerGrid = grid1.toSmrPower();

	Cochleagram* coch2 = new Cochleagram(*signal2, sampleRate);
	delete signal2;
	SignalGrid grid2(*(coch2->cochleagram), .020*sampleRate, .010*sampleRate);
	delete coch2;
	doubleGrid* powerGrid2 = grid2.toSmrPower();

	doubleGrid* decibelGrid = gridOfRelativeDecibels(*powerGrid, *powerGrid2);
	delete powerGrid;
	delete powerGrid2;

	/*for (int i = 0; i < decibelGrid->ROWS; i++) {
		for (int j = 0; j < decibelGrid->COLUMNS; j++) {
		print (int)(*decibelGrid)(i, j);
		}
		}
		print "" end;*/

	boolGrid* idealBinaryMask1 = createIdealBinaryMask(*decibelGrid, 1);
	FilterBank bank(128, 5500, 80, 44100);
	SignalBank* siggyBank1 = bank.filter(*signal1);
	SignalGrid siggyGrid1 = SignalGrid(*siggyBank1, .020*sampleRate, .010*sampleRate);
	for (int i = 0; i < idealBinaryMask1->ROWS; i++) {
		for (int j = 0; j < idealBinaryMask1->COLUMNS; j++) {
			if (!(*(idealBinaryMask1))(i, j))
				siggyGrid1.deleteCell(i, j);
		}
	}

	SignalBank resynthesisBank(128, 44100, siggyBank1->SAMPLES);
	for (int channel = 0; channel < siggyGrid1.CHANNELS; channel++) {
		Signal* sigChan = new Signal(siggyBank1->SAMPLES, siggyBank1->SAMPLE_RATE);//new double[siggyBank1->SAMPLES] {};
		for (int frame = 0; frame < siggyGrid1.FRAMES; frame++) {
			int startSample = frame * siggyGrid1.FRAME_OFFSET;
			for (int sample = startSample; sample < startSample + siggyGrid1.FRAME_SIZE; sample++) {
				double samp = siggyGrid1[frame][channel][sample% siggyGrid1.FRAME_SIZE];
				double presamp = (*sigChan)[sample];
				(*sigChan)[sample] = samp + presamp;
			}
		}
		resynthesisBank.add(sigChan, channel);
	}

	double* resynthesized = new double[siggyBank1->SAMPLES] {};
	for (int channel = 0; channel < 128; channel++) {
		for (int sample = 0; sample < siggyBank1->SAMPLES; sample++) {
			resynthesized[sample] += resynthesisBank[channel][sample];
		}
	}

	double largest = 0;
	for (int sample = 0; sample < siggyBank1->SAMPLES; sample++) {
		double sam = resynthesized[sample];
		if (sam > largest) {
			largest = sam;
		}
	}

	for (int sample = 0; sample < siggyBank1->SAMPLES; sample++) {
		double sam = resynthesized[sample];
		resynthesized[sample] = sam / largest;
	}

	FILE* results = fopen("Result.wav", "wb");
	fwrite(resynthesized, sizeof(double), siggyBank1->SAMPLES, results);



	//decibelGrid->negate();
	//boolGrid* idealBinaryMask2 = createIdealBinaryMask(*decibelGrid, 1);

	//for (int i = 0; i < idealBinaryMask1->ROWS; i++) {
	//	for (int j = 0; j < idealBinaryMask1->COLUMNS; j++) {
	//		print (*idealBinaryMask1)(i, j);
	//	}
	//	for (int k = 0; k < 1000000; k++) {
	//		int a = 5;
	//		a--;
	//	}
	//}
	//print "" end;
}

