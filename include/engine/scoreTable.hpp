#ifndef SCORE_TABLE_H
#define SCORE_TABLE_H

#include <vector>
#include <string>

namespace ScoreTable
{
	std::vector<std::pair<std::string, int>> readPlayerScoreTable();
	bool savePlayerScoreTable(std::vector<std::pair<std::string, int>> scoreTable);
};

#endif
