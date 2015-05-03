#pragma once
class GlobalInhibitor
{
public:
	double inhibition = 0;

	GlobalInhibitor();
	void update(double stepSize, bool isOscOverThreshold);
	~GlobalInhibitor();
};

