#pragma once

class AudioFile
{
private:
	FILE* file;
	int seekToData();
public:
	AudioFile(FILE* file);
	short* getSamples(int& dataSize);
	~AudioFile();
};

