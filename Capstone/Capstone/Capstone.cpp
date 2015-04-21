// Capstone.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "definitions.h"
#include<fstream>

#include "testing.h"
#include "SignalBank.h"
#include "SignalGrid.h"

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

	signal1->trim(signal2->SAMPLES);

	int sampleRate = 44100;
	int channels = 32;

	//GammatoneFilter filter(7500, 630, 44100);
	//Signal* simpleFilt = filter.filter(*signal1);

	//FilterBank bankb(256, 100, 20000, 44100);
	//SignalBank* siggyb = bankb.filter(*signal1);


	//double* resynthedbanksig1 = new double[siggyb->SAMPLES] {};
	//for (int channel = 0; channel < 256; channel++) {
	//	for (int sample = 0; sample < siggyb->SAMPLES; sample++) {
	//		resynthedbanksig1[sample] += (*siggyb)[channel][sample];
	//	}
	//}

	//FILE* results = fopen("Results.wav", "wb");

	//for (int i = 0; i < 64; i++) {
	//	std::string path = "Result" + std::to_string(i) + ".wav";
	//	FILE* results = fopen(path.c_str(), "wb");
	//	fwrite((*siggyb)[i].signal, sizeof(double), siggyb->SAMPLES, results);
	//}
	//fwrite(simpleFilt->signal, sizeof(double), simpleFilt->SAMPLES, results);
	//fwrite(resynthedbanksig1, sizeof(double), simpleFilt->SAMPLES, results);



	Signal* mixed = staticTools::combine(*signal1, *signal2);
	mixed->normalize();
	FILE* mixF = fopen("Mixed.wav", "wb");
	fwrite(mixed->signal, sizeof(double), mixed->SAMPLES, mixF);

	signal1->reverse(); 

	
	Cochleagram* coch1 = new Cochleagram(*signal1, sampleRate);

	for (int channel = 0; channel < coch1->CHANNELS; channel++) {
		(*(coch1->cochleagram))[channel].reverse();
	}
	//////delete signal1;
	SignalGrid* grid1 = new SignalGrid(*(coch1->cochleagram), .020*sampleRate, .010*sampleRate);
	print grid1->CHANNELS << '\t' << grid1->FRAMES end;
	delete coch1;
	doubleGrid* powerGrid = grid1->toSmrPower();
	delete grid1;


	signal2->reverse();
	Cochleagram* coch2 = new Cochleagram(*signal2, sampleRate);
	for (int channel = 0; channel < coch1->CHANNELS; channel++) {
		(*(coch2->cochleagram))[channel].reverse();
	}
	delete signal2;
	SignalGrid* grid2 = new SignalGrid(*(coch2->cochleagram), .020*sampleRate, .010*sampleRate);
	print grid2->CHANNELS << '\t' << grid2->FRAMES end;
	delete coch2;
	doubleGrid* powerGrid2 = grid2->toSmrPower();
	delete grid2;

	//for (int i = 0; i < powerGrid2->ROWS; i++) {
	//	print minInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) << '\t' << maxInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) << '\t' << avgInArray((*powerGrid2).grid[i], powerGrid2->COLUMNS) end;
	//	print minInArray((*powerGrid).grid[i], powerGrid->COLUMNS) << '\t' << maxInArray((*powerGrid).grid[i], powerGrid->COLUMNS) << '\t' << avgInArray((*powerGrid).grid[i], powerGrid->COLUMNS) end;
	//}


	//print powerGrid->COLUMNS << '\t' << powerGrid->ROWS end;
	//print powerGrid2->COLUMNS << '\t' << powerGrid2->ROWS end;


	doubleGrid* decibelGrid = gridOfRelativeDecibels(*powerGrid2, *powerGrid); //simpleRatio(*powerGrid, *powerGrid2);//diffGrid(*powerGrid, *powerGrid2);//gridOfRelativeDecibels(*powerGrid, *powerGrid2);
	delete powerGrid;
	delete powerGrid2;


	boolGrid* idealBinaryMask1 = createIdealBinaryMask(*decibelGrid, 3);

	for (int i = 0; i < decibelGrid->ROWS; i++) {
		print minInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << maxInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) << '\t' << avgInArray((*decibelGrid).grid[i], decibelGrid->COLUMNS) end;
	}





	FilterBank bank(channels, 100, 8000, 44100);
	SignalBank* mixedBank = bank.filter(*mixed);
	
	SignalGrid mixedGrid = SignalGrid(*mixedBank, .020*sampleRate, .010*sampleRate);
	for (int frame = 0; frame < decibelGrid->ROWS; frame++) {
		for (int channel = 0; channel < decibelGrid->COLUMNS; channel++) {
			//mixedGrid[frame][channel].scale((*decibelGrid)(frame, channel));
			if (!(*(idealBinaryMask1))(frame, channel))
				mixedGrid.deleteCell(frame, channel);
		}
	}
	for (int frame = decibelGrid->ROWS; frame < mixedGrid.FRAMES; frame++) {
		for (int channel = decibelGrid->COLUMNS; channel < mixedGrid.CHANNELS; channel++) {
			mixedGrid[frame][channel].scale(0);
			/*if (!(*(idealBinaryMask1))(frame, channel))*/
			//mixedGrid.deleteCell(frame, channel);
		}
	}


	//FRAMES = SAMPLES / FRAME_SIZE + SAMPLES / FRAME_SIZE / 2 + (SAMPLES % FRAME_SIZE >= FRAME_OVERLAP ? 1 : 0);
	int trueSamples = mixedGrid.SAMPLES;//(siggyGrid1.FRAMES / 2 + (siggyGrid1.SAMPLES % siggyGrid1.FRAME_SIZE < siggyGrid1.FRAME_OVERLAP ? 1 : 0)) * siggyGrid1.FRAME_SIZE;

	SignalBank resynthesisBank(mixedGrid.CHANNELS, mixedGrid.SAMPLE_RATE, mixedGrid.SAMPLES);
	for (int i = 0; i < channels; i++) {
		resynthesisBank.add(new Signal(mixedGrid.SAMPLES, mixedGrid.SAMPLE_RATE), i);
	}

	for (int frame = 0; frame < mixedGrid.FRAMES; frame++) {
		for (int channel = 0; channel < mixedGrid.CHANNELS; channel++) {
			for (int sample = 0; sample < mixedGrid.FRAME_SIZE; sample++) {
				resynthesisBank[channel][sample + mixedGrid.FRAME_OFFSET * frame] += mixedGrid[frame][channel][sample];
			}
		}
	}

	//SignalBank resynthesisBank(channels, 44100, trueSamples);
	//for (int channel = 0; channel < siggyBank1->CHANNELS; channel++) {
	//	Signal* sigChan = new Signal(trueSamples, siggyBank1->SAMPLE_RATE);//new double[siggyBank1->SAMPLES] {};
	//	for (int frame = 0; frame < siggyGrid1.FRAMES; frame++) {
	//		//int startSample = frame * siggyGrid1.FRAME_OFFSET;
	//		//for (int sample = startSample; sample < startSample + siggyGrid1.FRAME_SIZE; sample++) {
	//		for (int sample = 0; sample < siggyGrid1.FRAME_SIZE; sample++) {
	//			double samp = siggyGrid1[frame][channel][sample];
	//			double presamp = (*sigChan)[sample + siggyGrid1.FRAME_OFFSET * frame];
	//			(*sigChan)[sample + siggyGrid1.FRAME_OFFSET * frame] = samp +presamp;
	//		}
	//	}
	//	resynthesisBank.add(sigChan, channel);
	//}

	Signal resynthesized(trueSamples, resynthesisBank.SAMPLE_RATE);
	for (int channel = 0; channel < channels; channel++){//=2) {
		for (int sample = 0; sample < trueSamples; sample++) {
			resynthesized[sample] += resynthesisBank[channel][sample];
		}
	}

	resynthesized.normalize();

	FILE* results = fopen("Result.wav", "wb");
	fwrite(resynthesized.signal, sizeof(double), trueSamples, results);



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

