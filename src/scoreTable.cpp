#include "scoreTable.hpp"
#include "fileUtils.hpp"
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <sstream>

std::vector<std::pair<std::string, int>> ScoreTable::readPlayerScoreTable()
{
	std::vector<std::pair<std::string, int>> scoreTable;
	std::string folderPath;
	std::string filePath;

	folderPath = File::getGameFolderPath();

	if (folderPath == "")
	{
		SDL_Log("ScoreTable: Failed to get the path to the game folder.");

		return scoreTable;
	}

	filePath = folderPath + "/scoreTable.txt";

	//Only read if the file exists
	if (SDL_GetPathInfo(filePath.c_str(), NULL))
	{
		File::ReadFileData *fileData;
		char *text;
		size_t size;
		std::string fileContents;

		fileData = File::readFile(filePath);

		if (fileData == NULL)
		{
			SDL_Log("ScoreTable: Failed to read score table file.");
			
			return scoreTable;
		}

		text = static_cast<char *>(fileData->data);
		size = fileData->bytes;

		fileContents.assign(text, size);

		File::freeReadFileData(fileData);

		//Add each score from our score table
		std::istringstream stream(fileContents);
		std::string name;
		int score;

		while (stream >> name >> score)
		{
		    scoreTable.emplace_back(name, score);
		}
	}

	return scoreTable;
}

bool ScoreTable::savePlayerScoreTable(std::vector<std::pair<std::string, int>> scoreTable)
{
	bool success;
	std::string folderPath;
	std::string filePath;
	SDL_IOStream *file;

	folderPath = File::getGameFolderPath();

	if (folderPath == "")
	{
		SDL_Log("ScoreTable: Failed to get the path to the game folder.");

		return false;
	}

	filePath = folderPath + "/scoreTable.txt";

	file = SDL_IOFromFile(filePath.c_str(), "w+");

	if (file == NULL)
        {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "ScoreTable: Failed to open scoreTable file for writing. Error: %s", SDL_GetError());

                return false;
        }

	success = true;

	for (const auto& entry : scoreTable)
	{
		size_t dataSize, writtenBytes;

		std::string line = entry.first + " " + std::to_string(entry.second) + "\n";

		dataSize = line.size();
		writtenBytes = SDL_WriteIO(file, line.data(), dataSize);

		if (writtenBytes < dataSize)
		{
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "ScoreTable: Error occured when writing to scoreTable file. Error: %s", SDL_GetError());
			success = false;
			break;
		}
	}

	SDL_CloseIO(file);

	return success;
}
