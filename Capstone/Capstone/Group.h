#pragma once
#include "Definitions.h"
#include <vector>
#include "Segment.h"

class Group
{
public:
	std::vector<Segment*>* foreground;
	std::vector<Segment*>* background;

	Group();
	void addForeground(Segment* seg);
	void addBackground(Segment* seg);
	boolGrid* foregroundMask(int channels, int frames);
	boolGrid* backgroundMask(int channels, int frames);
	void deleteGroup();
	~Group();
};

