#include "stdafx.h"
#include <random>

#include "Definitions.h"


double maxInArray(double* darray, int size) {
	double max = -9999999999;
	for (int i = 0; i < size; i++) {
		if (darray[i] > max) max = darray[i];
	}
	return max;
}

double minInArray(double* darray, int size) {
	double min = 99999999999;
	for (int i = 0; i < size; i++) {
		if (darray[i] < min) min = darray[i];
	}
	return min;
}

double avgInArray(double* darray, int size) {
	double avg = 0;
	for (int i = 0; i < size; i++) {
		double value = darray[i];
		avg += darray[i];
	}
	return avg / size;
}


namespace staticTools{
	using byte = unsigned char;

	Signal* makeWave(int length, int sampleRate, int frequency) {
		Signal* wave = new Signal(length, sampleRate);//double[length];

		for (int i = 0; i < length; i++) {
			(*wave)[i] = sin(PI * 2 * frequency * i / length);
		}
		return wave;
	}

	Signal* normalize(short* data, int dataSize) {
		double largestSample = 0;

		for (int i = 0; i < dataSize; i++) {
			largestSample = largestSample > data[i] ? largestSample : data[i];
		}

		Signal* sig = new Signal(dataSize, 44100);

		for (int i = 0; i < dataSize; i++) {
			(*sig)[i] = data[i] / largestSample;
			//print (*sig)[i] << '\t' << data[i] end;
		}

		delete[] data;
		return sig;
	}

	int seekToData(FILE* source) {

		char descChars[5];

		bool datafound = false;
		bool firstTime = true;
		int dataSize = 0;
		while (!feof(source) && !datafound) {
			fread(descChars, sizeof(char), 4, source);

			if (!firstTime){
				if (descChars[0] == 'd' && descChars[1] == 'a' && descChars[2] == 't' && descChars[3] == 'a') {
					datafound = true;
				}

				fread(descChars, sizeof(char), 4, source);
			}
			if (datafound) {
				std::cout << "";
			}
			int skip = firstTime ? 8 :
				((descChars[0] & 0xFF)
				| (descChars[1] & 0xFF) << 8
				| (descChars[2] & 0xFF) << 16
				| (descChars[3] & 0xFF) << 24);

			if (datafound) {
				dataSize = skip;
			}
			else {
				fseek(source, skip, SEEK_CUR);
			}
			firstTime = false;
		}
		return dataSize;
	}

	Signal* readWav(std::string& fileName) {
		FILE* file = fopen(fileName.c_str(), "rb");
		print file << fileName end;
		int dataSize = seekToData(file);
		dataSize = dataSize / 2; // for 8 bit ints
		short* data = new short[dataSize];
		fread(data, sizeof(short), dataSize, file);
		fclose(file);
		Signal* sig = new Signal(dataSize, 44100);
		for (int i = 0; i < dataSize; i++) {
			(*sig)[i] = data[i];
		}
		return normalize(data, dataSize);
	}

	Signal* combine(Signal& s1, Signal& s2) {
		int size = MAX(s1.SAMPLES, s2.SAMPLES);
		int overlap = MIN(s1.SAMPLES, s2.SAMPLES);
		Signal* combo = new Signal(size, s1.SAMPLE_RATE);

		for (int i = 0; i < overlap; i++) {
			(*combo)[i] = s1[i] + s2[i];
		}

		Signal& largerSig = s1.SAMPLES > s2.SAMPLES ? s1 : s2;
		for (int i = overlap; i < size; i++) {
			(*combo)[i] = largerSig[i];
		}
		return combo;
	}

	double standardDeviation(Signal& signal, double mean) {
		double sum = 0;
		for (int i = 0; i < signal.SAMPLES; i++) {
			sum += pow((signal[i] - mean), 2);
		}
		return sqrt(sum / signal.SAMPLES);
	}

	/*std::random_device rd;
	std::mt19937 e2(rd());*/
	std::default_random_engine generator;
	std::normal_distribution<double> d(-.02, .02);
	
	double nonZeroGaussianRandom(double minMax) {
	/*	double randv;
		do{
			randv = rand();
		} while (randv == 0.0);

		double randlog = log((double)randv / RAND_MAX);
		double randomNumber = pow(-2 * randlog, .5) * cos(2 * PI * rand()) * minMax - minMax;

		return randomNumber;*/
		
		double result = d(generator);
		return result;
	}


	//void readChunk(FILE* source, char* chunkName, byte* chunkBuffer) {
	//	fseek(source, 0, SEEK_SET);

	//	char* description = new char[5];
	//	description[4] = '\0';
	//	byte* chunkSize = new byte[4];

	//	bool firstTime = true
	//	while (true) {
	//		fread(description, sizeof(char), 4, source);
	//		if (!firstTime) {
	//			if (description == chunkName) {
	//				fread(chunkSize, sizeof(byte), 4, source);
	//			}
	//		}
	//	}
	//}


}