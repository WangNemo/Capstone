#include "stdafx.h"
#include "Group.h"


Group::Group()
{
	foreground = new std::vector<Segment*>();
	background = new std::vector<Segment*>();
}

void Group::addForeground(Segment* seg) {
	foreground->push_back(seg);
}

void Group::addBackground(Segment* seg) {
	background->push_back(seg);
}

void Group::deleteGroup() {
	delete foreground;
	delete background;
}

boolGrid* Group::foregroundMask(int channels, int frames) {
	bool** grid = new bool*[channels];
	for (int channel = 0; channel < channels; channel++) {
		grid[channel] = new bool[frames];
		std::fill_n(grid[channel], frames, false);
	}

	for (Segment* seg : (*foreground)) {
		for (int i = 0; i < seg->segmentSize; i++) {
			RowColumn* rc = seg->segment[i];
			grid[rc->row][rc->col] = true;
		}
	}

	return new boolGrid(grid, channels, frames);
}

boolGrid* Group::backgroundMask(int channels, int frames) {
	bool** grid = new bool*[channels];
	for (int channel = 0; channel < channels; channel++) {
		grid[channel] = new bool[frames];
		std::fill_n(grid[channel], frames, false);
	}

	for (Segment* seg : (*background)) {
		for (int i = 0; i < seg->segmentSize; i++) {
			RowColumn* rc = seg->segment[i];
			grid[rc->row][rc->col] = true;
		}
	}

	return new boolGrid(grid, channels, frames);
}


Group::~Group()
{
}
