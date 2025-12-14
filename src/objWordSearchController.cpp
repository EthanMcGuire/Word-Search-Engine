#include "objWordSearchController.hpp"
#include "bitmapFont.hpp"
#include "assetManager.hpp"
#include "gameManager.hpp"
#include "objWordSearchBox.hpp"
#include "objGameClock.hpp"
#include "config.hpp"
#include "random.hpp"
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <iostream>

ObjWordSearchController::ObjWordSearchController(GameManager *gameManager, double x, double y) : RenderableObject("objWordSearchController", gameManager, x, y)
{
	loadWords(Config::WORDS_PATH);

	font = gameManager->getAssetManager()->getBitmap("fntOpenSans");

	if (font == NULL)
	{
		throw std::runtime_error("ObjWordSearchController: Failed to load font.");
	}
	
	wordSearchBox = gameManager->createObject<ObjWordSearchBox>("objWordSearchBox", 480, 270);
	wordSearchBox->setReadyCallback(std::bind(&ObjWordSearchController::boxReadyCallback, this));
	wordSearchBox->setWordsFoundCallback(std::bind(&ObjWordSearchController::wordsFoundCallback, this, std::placeholders::_1));
	wordSearchBox->setWrongLetterCallback(std::bind(&ObjWordSearchController::wrongLetterCallback, this));

	gameClock = gameManager->createObject<ObjGameClock>("objGameClock", 896, 20);
	gameClock->setCallbackFunction(std::bind(&ObjWordSearchController::gameClockCompletedCallback, this));
	gameClock->setTime(STARTING_TIME);
	gameClock->startTimer();

	wordLengthScoreMapping.push_back({5, 250});
	wordLengthScoreMapping.push_back({10, 200});
	wordLengthScoreMapping.push_back({20, 150});
	wordLengthScoreMapping.push_back({50, 100});

	resetGameData();	
	startNextLevel();
}

void ObjWordSearchController::update(double deltaTime)
{
	switch (state)
	{
		case WordSearchState::WORD_SEARCH_STATE_WAITING_FOR_BOX:
		{
			//Waiting for a callback
		}
		break;

		case WordSearchState::WORD_SEARCH_STATE_ACTIVE_GAME:
		{
			//Waiting for game over, or for box to have all words found	
		}
		break;
		
		case WordSearchState::WORD_SEARCH_STATE_GAME_OVER:
		{
			//Wait for player to retry
			//Upon retry:
			//resetGameData();	
			//startNextLevel();
		}
		break;
	}
}

void ObjWordSearchController::renderGui(SDL_Renderer *renderer)
{
	std::string text = "";
	int drawX, drawY;

	drawX = 4;
	drawY = 4;

	text = std::string("LEVEL: ") + std::to_string(level);
	font->drawTextOutlined(renderer, drawX, drawY, text, {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::TOP);
	drawY += font->getTextHeight(text) + GUI_WORD_SEP_Y;
	
	text = std::string("SCORE: ") + std::to_string(score);
	font->drawTextOutlined(renderer, drawX, drawY, text, {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::TOP);
	drawY += font->getTextHeight(text) + GUI_WORD_SEP_Y;

	text = "WORDS";
	font->drawTextOutlined(renderer, drawX, drawY, text, {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::TOP);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderLine(renderer, drawX, drawY + 2 + font->getTextHeight(text), drawX + font->getTextWidth(text), drawY + 2 + font->getTextHeight(text));

	drawX += GUI_WORDS_OFFSET_X;
	drawY += font->getTextHeight(text) + GUI_WORDS_OFFSET_Y;

	//Only draw words if we are in an active game
	if (state == WordSearchState::WORD_SEARCH_STATE_ACTIVE_GAME)
	{
		for (int i = 0; i < currentWords.size(); i++)
		{
			int textHeight;
			SDL_Color drawColor;
			
			text = currentWords[i];
			textHeight = font->getTextHeight(text);

			if (!wordsFound[i])
			{
				drawColor = {255, 255, 255, 255};
			}
			else
			{
				drawColor = {255, 0, 0, 255};
			}

			font->drawTextOutlined(renderer, drawX, drawY, text, drawColor, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::TOP);

			if (wordsFound[i])
			{
				//FOUND! CROSS OUT
				SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
				SDL_RenderLine(renderer, drawX, drawY + textHeight / 2, drawX + font->getTextWidth(text), drawY + textHeight / 2);
			}

			drawY += textHeight + 4;
		}
	}

}

void ObjWordSearchController::resetGameData()
{
	level = 0;
	score = 0;
	difficulty = 0;
}

void ObjWordSearchController::loadWords(std::string path)
{
	if (!std::filesystem::exists(path))
	{
		throw std::runtime_error("ObjWordSearchController: Failed to load words. File does not exist! Path: " + path);
	}

	std::ifstream fin;

	fin.open(path);

	if (fin.is_open())
	{
		std::string line;

		SDL_Log("ObjWordSearchController: Loading words...");

		while (std::getline(fin, line))
		{
			int length;

			line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
			std::transform(line.begin(), line.end(), line.begin(), ::toupper);

			length = line.length();
			
			if (words.find(length) == words.end())
			{
				words[length] = std::vector<std::string>();
			}

			words[length].push_back(line);
		}

		SDL_Log("ObjWordSearchController: Finished loading words.");

		/*
		for (int i = 0; i < words[3].size(); i++)
		{
			SDL_Log("%s", words[3][i].c_str());
		}

		for (const auto& pair : words) {
			std::cout << pair.first << std::endl;
		    }
		    */
	}
	else
	{
		throw std::runtime_error("ObjWordSearchController: Failed to load words. Failed to read words file. Path: " + path);
	}
}

void ObjWordSearchController::startNextLevel()
{
	level++;
	difficulty = SDL_min(difficulty + 1, MAX_DIFFICULTY);
	gridSize = STARTING_GRID_SIZE + (difficulty - 1) * 2;
	wordCountMin = STARTING_MIN_WORD_COUNT + (difficulty / 4);
	wordCountMax = STARTING_MAX_WORD_COUNT + (difficulty / 4);
	
	SDL_Log("ObjWordSearchController: Starting next level. Level: %d, Difficulty: %d", level, difficulty);

	getWords();

	wordSearchBox->initializeGrid(gridSize, currentWords);
	gameClock->pauseTimer();
	
	state = WordSearchState::WORD_SEARCH_STATE_WAITING_FOR_BOX;
}

void ObjWordSearchController::getWords()
{
	int wordCount;
	Random *random;

	SDL_Log("ObjWordSearchController: Getting words...");

	currentWords.clear();
	wordsFound.clear();

	random = gameManager->getRandom();
	wordCount = random->getRandomInt(wordCountMin, wordCountMax);

	for (int i = 0; i < wordCount; i++)
	{
		int length;
		std::string word;

		length = random->getRandomInt(MIN_WORD_LENGTH, SDL_min(gridSize, MAX_WORD_LENGTH));

		do
		{
			int index;

			index = random->getRandomInt(0, words[length].size() - 1);
			word = words[length][index];
		}
		while (std::find(currentWords.begin(), currentWords.end(), word) != currentWords.end());
		
		currentWords.push_back(word);
		wordsFound.push_back(false);
	} 

	SDL_Log("ObjWordSearchController: Got words.");
}

void ObjWordSearchController::boxReadyCallback()
{
	gameClock->unpauseTimer();

	state = WordSearchState::WORD_SEARCH_STATE_ACTIVE_GAME;
}

void ObjWordSearchController::gameClockCompletedCallback()
{
	//TODO
	//Start game over sequence
	state = WordSearchState::WORD_SEARCH_STATE_GAME_OVER;

	wordSearchBox->gameOver();
	
	//Won't do this normally!!!
	gameManager->setRoomToLoad("titlescreen");
}

/// @brief Called by the WordSearchBox when the wrong letter is selected.
void ObjWordSearchController::wrongLetterCallback()
{
	gameClock->removeTime(WRONG_LETTER_TIME_LOSS);
}

/// @brief Called by the WordSearchBox when one or more words were found.
/// @param words The list of words that were found.
void ObjWordSearchController::wordsFoundCallback(std::vector<std::string> words)
{
	SDL_Log("ObjWordSearchController: Found %ld words!", words.size());

	//Set words as found
	for (int i = 0; i < currentWords.size(); i++)
	{
		if (std::find(words.begin(), words.end(), currentWords[i]) != words.end())
		{
			if (wordsFound[i])
			{
				SDL_Log("ObjWordSearchController: Warning! For some reason a word is being marked as found more than once. Word: %s", currentWords[i].c_str());
			}

			wordsFound[i] = true;
		}
	}

	int scoreToAdd;
	float multiplier;

	//Calculate and add score
	scoreToAdd = 0;
	multiplier = 1;

	for (std::string word : words)
	{
		int wordScore;

		wordScore = getWordLengthScore(word.length());

		scoreToAdd += wordScore;
	}

	multiplier += (difficulty - 1) * DIFFICULTY_MULTIPLIER;
	multiplier += (words.size() - 1) * WORD_BONUS_MULTIPLIER;

	SDL_Log("ObjWordSearchController: Adding score. Score: %d, Multiplier: %f", scoreToAdd, multiplier);

	addScore((int) scoreToAdd * multiplier);

	//Add clock time, based on the number of words
	gameClock->addTime(CORRECT_WORD_TIME_ADD * words.size());
}

int ObjWordSearchController::getWordLengthScore(int length)
{
	for (std::pair<int, int> pair : wordLengthScoreMapping)
	{
		if (length <= pair.first)
		{
			return pair.second;
		}
	}

	return wordLengthScoreMapping.back().second;
}

void ObjWordSearchController::addScore(int scoreToAdd)
{
	//TODO
	//Add score over time instead
	//TODO
	//Make a more vibrant effect based on the amount of score being added 
	
	score += scoreToAdd;
}
