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
	gameClock = gameManager->createObject<ObjGameClock>("objGameClock", 896, 20);
	gameClock->setCallbackFunction(std::bind(&ObjWordSearchController::gameClockCompletedCallback, this));
	gameClock->setTime(STARTING_TIME);
	gameClock->startTimer();

	level = 0;
	score = 0;
	difficulty = 0;

	startState(WordSearchState::WORD_SEARCH_STATE_START_NEXT_LEVEL);
}

void ObjWordSearchController::update(double deltaTime)
{
	switch (state)
	{
		case WordSearchState::WORD_SEARCH_STATE_START_NEXT_LEVEL:
		{
			//Nothing to do
		}
		break;
		
		case WordSearchState::WORD_SEARCH_STATE_WAITING_FOR_BOX:
		{
			//Waiting for a callback?
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
		}
		break;
	}
}

void ObjWordSearchController::renderGui(SDL_Renderer *renderer)
{
	std::string text = "";

	text += std::string("LEVEL: ") + std::to_string(level) + "/n";
	text += std::string("SCORE: ") + std::to_string(score) + "/n";
	text += "WORDS/n";

	for (std::string word : currentWords)
	{
		text += " ";
		text += word + "/n";
	}	

	font->drawTextAligned(renderer, 4, 4, text, {255, 255, 255, 255}, TextAlign::LEFT, TextAlign::TOP);
	font->drawTextOutlined(renderer, 4, 4, text, {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::TOP);
}

void ObjWordSearchController::startState(WordSearchState nextState)
{
	state = nextState;

	switch (state)
	{
		case WordSearchState::WORD_SEARCH_STATE_START_NEXT_LEVEL:
		{
			startNextLevel();

			gameClock->pauseTimer();
			startState(WordSearchState::WORD_SEARCH_STATE_WAITING_FOR_BOX);
		}
		break;
		
		case WordSearchState::WORD_SEARCH_STATE_WAITING_FOR_BOX:
		{

		}
		break;

		case WordSearchState::WORD_SEARCH_STATE_ACTIVE_GAME:
		{
			gameClock->unpauseTimer();
		}
		break;
		
		case WordSearchState::WORD_SEARCH_STATE_GAME_OVER:
		{
			//TODO
			//Disable the box so the player can't click it
		}
		break;
	}
}

void ObjWordSearchController::gameClockCompletedCallback()
{
	gameManager->setRoomToLoad("titlescreen");
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
	difficulty++;
	gridSize = STARTING_GRID_SIZE + (difficulty - 1) * 2;
	wordCountMin = STARTING_MIN_WORD_COUNT + (difficulty / 4);
	wordCountMax = STARTING_MAX_WORD_COUNT + (difficulty / 4);
	
	SDL_Log("ObjWordSearchController: Starting next level. Level: %d, Difficulty: %d", level, difficulty);

	getWords();

	wordSearchBox->initializeGrid(gridSize, currentWords);
}

void ObjWordSearchController::getWords()
{
	int wordCount;
	Random *random;

	SDL_Log("ObjWordSearchController: Getting words...");

	currentWords.clear();

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
	} 

	SDL_Log("ObjWordSearchController: Got words.");
}
