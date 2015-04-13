#include "stdafx.h"
#include "AudioFile.h"

AudioFile::AudioFile(FILE* file) : file(file)
{
}

int AudioFile::seekToData() {
	char descChars[5];

	bool datafound = false;
	bool firstTime = true;
	int dataSize = 0;
	while (!feof(file) && !datafound) {
		fread(descChars, sizeof(char), 4, file);

		if (!firstTime){
			if (descChars[0] == 'd' && descChars[1] == 'a' && descChars[2] == 't' && descChars[3] == 'a') {
				datafound = true;
			}

			fread(descChars, sizeof(char), 4, file);
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
			fseek(file, skip, SEEK_CUR);
		}
		firstTime = false;
	}
	return dataSize;
}

short* AudioFile::getSamples(int& dataSize) {
	int dataSizeInBytes = seekToData();
	dataSize = dataSizeInBytes / 2;
	short* data = new short[dataSize];
	fread(data, sizeof(short), dataSize, file);
	return data;
}


AudioFile::~AudioFile()
{
}
