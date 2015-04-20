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
			decibelGrid[frame][channel] = one / two;
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

	int sampleRate = 44100;
	int channels = 128;

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
	//FILE* mixF = fopen("Mixed.wav", "wb");
	//fwrite(mixed->signal, sizeof(double), mixed->SAMPLES, mixF);

	Cochleagram* coch1 = new Cochleagram(*signal1, sampleRate);
	//////delete signal1;
	SignalGrid* grid1 = new SignalGrid(*(coch1->cochleagram), .020*sampleRate, .010*sampleRate);
	print grid1->CHANNELS << '\t' << grid1->FRAMES end;
	delete coch1;
	doubleGrid* powerGrid = grid1->toSmrPower();
	delete grid1;

	Cochleagram* coch2 = new Cochleagram(*signal2, sampleRate);
	delete signal2;
	SignalGrid* grid2 = new SignalGrid(*(coch2->cochleagram), .020*sampleRate, .010*sampleRate);
	print grid2->CHANNELS << '\t' << grid2->FRAMES end;
	delete coch2;
	doubleGrid* powerGrid2 = grid2->toSmrPower();
	delete grid2;

	print powerGrid->COLUMNS << '\t' << powerGrid->ROWS end;
	print powerGrid2->COLUMNS << '\t' << powerGrid2->ROWS end;


	doubleGrid* decibelGrid = simpleRatio(*powerGrid, *powerGrid2);//diffGrid(*powerGrid, *powerGrid2);//gridOfRelativeDecibels(*powerGrid, *powerGrid2);
	delete powerGrid;
	delete powerGrid2;

	//boolGrid* idealBinaryMask1 = createIdealBinaryMask(*decibelGrid, -6);

	//print idealBinaryMask1->COLUMNS << '\t' << idealBinaryMask1->ROWS end;



	/*for (int i = 0; i < decibelGrid->ROWS; i++) {
		for (int j = 0; j < decibelGrid->COLUMNS; j++) {
		print (int)(*decibelGrid)(i, j);
		}
		}
		print "" end;*/



	FilterBank bank(channels, 100, 8000, 44100);
	//mixed->reverse(); // reversed inside the cochleagram <- no more
	SignalBank* siggyBank1 = bank.filter(*mixed);
	
	//for (int channel = 0; channel < siggyBank1->CHANNELS; channel++) {
	//	(*siggyBank1)[channel].reverse();
	//}

	SignalGrid siggyGrid1 = SignalGrid(*siggyBank1, .020*sampleRate, .010*sampleRate);
	for (int frame = 0; frame < decibelGrid->ROWS; frame++) {
		for (int channel = 0; channel < decibelGrid->COLUMNS; channel++) {
			siggyGrid1[frame][channel].scale((*decibelGrid)(frame, channel));
			/*if (!(*(idealBinaryMask1))(frame, channel))
				siggyGrid1.deleteCell(frame, channel);*/
		}
	}

	//FRAMES = SAMPLES / FRAME_SIZE + SAMPLES / FRAME_SIZE / 2 + (SAMPLES % FRAME_SIZE >= FRAME_OVERLAP ? 1 : 0);
	int trueSamples = siggyGrid1.SAMPLES;//(siggyGrid1.FRAMES / 2 + (siggyGrid1.SAMPLES % siggyGrid1.FRAME_SIZE < siggyGrid1.FRAME_OVERLAP ? 1 : 0)) * siggyGrid1.FRAME_SIZE;

	SignalBank resynthesisBank(siggyGrid1.CHANNELS, siggyGrid1.SAMPLE_RATE, siggyGrid1.SAMPLES);
	for (int i = 0; i < channels; i++) {
		resynthesisBank.add(new Signal(siggyGrid1.SAMPLES, siggyGrid1.SAMPLE_RATE), i);
	}

	for (int frame = 0; frame < siggyGrid1.FRAMES; frame++) {
		for (int channel = 0; channel < siggyGrid1.CHANNELS; channel++) {
			for (int sample = 0; sample < siggyGrid1.FRAME_SIZE; sample++) {
				resynthesisBank[channel][sample + siggyGrid1.FRAME_OFFSET * frame] += siggyGrid1[frame][channel][sample];
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

