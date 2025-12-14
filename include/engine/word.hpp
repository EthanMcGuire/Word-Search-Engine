#ifndef WORD_H
#define WORD_H

#include <string>
#include <vector>

struct Word
{
	std::string word;
	bool found = false;
	std::vector<std::pair<int, int>> letterLocations = {};
};

#endif
