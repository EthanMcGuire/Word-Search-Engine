#include "objWordSearchController.hpp"
#include "bitmapFont.hpp"
#include "assetManager.hpp"
#include "gameManager.hpp"
#include "objWordSearchBox.hpp"
#include "objGameClock.hpp"
#include "config.hpp"
#include "random.hpp"
#include "utility.hpp"
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

	//Calculate the GUI y offsets
	difYOffset = GUI_TEXT_OFFSET + font->getTextHeight(LEVEL_TEXT) + GUI_TEXT_SEP_Y; 
	scoreYOffset = difYOffset + font->getTextHeight(DIF_TEXT) + GUI_TEXT_SEP_Y; 
	wordsHeaderYOffset = scoreYOffset + font->getTextHeight(SCORE_TEXT) + GUI_TEXT_SEP_Y;
	wordsYOffset = wordsHeaderYOffset + font->getTextHeight(WORDS_TEXT) + GUI_WORDS_OFFSET_Y;
	
	//Create objects
	wordSearchBox = gameManager->createObject<ObjWordSearchBox>("objWordSearchBox", 480, 270);
	wordSearchBox->setReadyCallback(std::bind(&ObjWordSearchController::boxReadyCallback, this));
	wordSearchBox->setDoneShrinkingCallback(std::bind(&ObjWordSearchController::boxDoneShrinkingCallback, this));
	wordSearchBox->setWordsFoundCallback(std::bind(&ObjWordSearchController::wordsFoundCallback, this, std::placeholders::_1));
	wordSearchBox->setWrongLetterCallback(std::bind(&ObjWordSearchController::wrongLetterCallback, this));

	gameClock = gameManager->createObject<ObjGameClock>("objGameClock", 896, 20);
	gameClock->setCallbackFunction(std::bind(&ObjWordSearchController::gameClockCompletedCallback, this));
	gameClock->setTime(STARTING_TIME);
	gameClock->startTimer();

	//Score mappings
	wordLengthScoreMapping.push_back({5, 250});
	wordLengthScoreMapping.push_back({10, 200});
	wordLengthScoreMapping.push_back({20, 150});
	wordLengthScoreMapping.push_back({50, 100});

	//Start the game
	resetGameData();	
	beginNextLevelDelay();
}

void ObjWordSearchController::update(double deltaTime)
{
	switch (state)
	{
		case WordSearchState::WORD_SEARCH_STATE_NEXT_LEVEL_DELAY:
		{
			nextLevelDelay -= deltaTime * 1000;

			if (nextLevelDelay <= 0)
			{
				startNextLevel();
			}
		}
		break;

		case WordSearchState::WORD_SEARCH_STATE_STARTING_ROUND:
		{
			if (moveWordsToGoalLocation() && boxReady)
			{
				wordSearchBox->startRound();
				gameClock->unpauseTimer();

				state = WordSearchState::WORD_SEARCH_STATE_ACTIVE_GAME;
			}	
		}
		break;

		case WordSearchState::WORD_SEARCH_STATE_ACTIVE_GAME:
		{
			bool allWordsCompleted = true;

			for (WordInfo word : currentWords)
			{
				if (!word.found)
				{
					allWordsCompleted = false;
					break;
				}
			}

			//Round completed successfully?
			if (allWordsCompleted)
			{
				roundCompleted();
			}
		}
		break;

		case WordSearchState::WORD_SEARCH_STATE_ROUND_COMPLETED_DELAY:
		{
			endRoundDelay -= deltaTime * 1000;

			if (endRoundDelay <= 0)
			{
				endRound();
			}
		}
		break;

		case WordSearchState::WORD_SEARCH_STATE_ENDING_ROUND:
		{
			//Collapse words
			moveWordsToGoalLocation();

			//Delete words that have reached the SCORE text 
			for (int i = currentWords.size() - 1; i >= 0; i--)
			{
				if (SDL_abs(currentWords[i].y - currentWords[i].goalY) <= SDL_FLT_EPSILON)
				{
					//Add score
					createRoses(currentWords[i].x + currentWords[i].width / 2.0, currentWords[i].y + currentWords[i].height / 2.0, WORD_COMPLETED_SCORE_ADD);
					
					//Remove word
					currentWords.erase(currentWords.begin() + 1);
				}	
			}

			if (currentWords.size() == 0 && boxShrunk)
			{
				beginNextLevelDelay();
			}
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

	drawX = GUI_TEXT_OFFSET;
	drawY = GUI_TEXT_OFFSET;

	text = std::string(LEVEL_TEXT) + std::to_string(level);
	font->drawTextOutlined(renderer, drawX, drawY, text, {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::TOP);
	
	drawY = difYOffset;

	text = std::string(DIF_TEXT) + std::to_string(difficulty);
	font->drawTextOutlined(renderer, drawX, drawY, text, {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::TOP);

	drawY = scoreYOffset;

	text = std::string(SCORE_TEXT) + std::to_string(score);
	font->drawTextOutlined(renderer, drawX, drawY, text, {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::TOP);

	drawY = wordsHeaderYOffset;

	text = WORDS_TEXT;
	font->drawTextOutlined(renderer, drawX, drawY, text, {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::TOP);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderLine(renderer, drawX, drawY + 2 + font->getTextHeight(text), drawX + font->getTextWidth(text), drawY + 2 + font->getTextHeight(text));

	drawX = GUI_TEXT_OFFSET + GUI_WORDS_OFFSET_X;
	drawY = wordsYOffset;

	drawWords(renderer);
}

void ObjWordSearchController::drawWords(SDL_Renderer *renderer)
{
	for (WordInfo word : currentWords)
	{
		SDL_Color drawColor;
		
		if (!word.found)
		{
			drawColor = {255, 255, 255, 255};
		}
		else
		{
			drawColor = {255, 0, 0, 255};
		}

		font->drawTextOutlined(renderer, word.x, word.y, word.word, drawColor, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::TOP);

		if (word.found)
		{
			//FOUND! CROSS OUT
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
			SDL_RenderLine(renderer, word.x, word.y + word.height / 2, word.x + word.width, word.y + word.height / 2);
		}
	}	
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

void ObjWordSearchController::resetGameData()
{
	level = 0;
	score = 0;
	difficulty = 1;
}

void ObjWordSearchController::beginNextLevelDelay()
{
	state = WordSearchState::WORD_SEARCH_STATE_NEXT_LEVEL_DELAY;
	nextLevelDelay = NEXT_LEVEL_DELAY;
}

/// @brief Starts the next level. Increases difficulty, preps the WordSearchBox, pauses the game clock, and enters us into the STARTING_ROUND state.
void ObjWordSearchController::startNextLevel()
{
	std::vector<std::string> wordStrings;

	level++;

	if (level % DIFFICULTY_INCREASE_ROUND == 0)
	{
		difficulty = SDL_min(difficulty + 1, MAX_DIFFICULTY);
	}

	gridSize = STARTING_GRID_SIZE + (difficulty - 1) * 1;
	wordCountMin = STARTING_MIN_WORD_COUNT + (difficulty / 4);
	wordCountMax = STARTING_MAX_WORD_COUNT + (difficulty / 4);
	
	SDL_Log("ObjWordSearchController: Starting next level. Level: %d, Difficulty: %d", level, difficulty);

	getWords();

	for (WordInfo word : currentWords)
	{
		wordStrings.push_back(word.word);
	}

	wordSearchBox->initializeGrid(gridSize, wordStrings);
	boxReady = false;
	gameClock->pauseTimer();
	
	state = WordSearchState::WORD_SEARCH_STATE_STARTING_ROUND;
}

void ObjWordSearchController::getWords()
{
	int wordCount;
	Random *random;
	double wordX, wordY, goalY;

	SDL_Log("ObjWordSearchController: Getting words...");

	currentWords.clear();

	random = gameManager->getRandom();
	wordCount = random->getRandomInt(wordCountMin, wordCountMax);

	wordX = GUI_TEXT_OFFSET + GUI_WORDS_OFFSET_X;
	wordY = wordsHeaderYOffset;
	goalY = wordsYOffset;

	for (int i = 0; i < wordCount; i++)
	{
		int length;
		int width, height;
		std::string word;

		length = random->getRandomInt(MIN_WORD_LENGTH, SDL_min(gridSize, MAX_WORD_LENGTH));

		do
		{
			int index;

			index = random->getRandomInt(0, words[length].size() - 1);
			word = words[length][index];
		}
		while (std::find_if(currentWords.begin(), currentWords.end(), [word](WordInfo &nextWord) {
			       return nextWord.word == word;
		       }) != currentWords.end());
		
		width = font->getTextWidth(word);
		height = font->getTextHeight(word);

		currentWords.push_back({wordX, wordY, goalY, word, width, height, false});
		goalY += height + GUI_WORDS_SEP_Y;
	} 

	SDL_Log("ObjWordSearchController: Got words.");
}

void ObjWordSearchController::roundCompleted()
{
	wordSearchBox->completeRound();
	gameClock->pauseTimer();
	
	endRoundDelay = END_ROUND_DELAY;
	state = WordSearchState::WORD_SEARCH_STATE_ROUND_COMPLETED_DELAY;
}

void ObjWordSearchController::endRound()
{
	wordSearchBox->shrinkBox();
	boxShrunk = false;

	gameClock->addTime(ROUND_COMPLETED_TIME_ADD);
	
	//Set the words to go to the SCORE text
	for (int i = 0; i < currentWords.size(); i++)
	{
		currentWords[i].goalY = scoreYOffset;
	}

	state = WordSearchState::WORD_SEARCH_STATE_ENDING_ROUND;
}

bool ObjWordSearchController::moveWordsToGoalLocation()
{
	bool allWordsReachedGoal = true;

	//Slide the words to their correct positions
	for (int i = 0; i < currentWords.size(); i++)
	{
		currentWords[i].y = Utility::lerp(currentWords[i].y, currentWords[i].goalY, WORD_MOVE_LERP_RATE);

		if (SDL_abs(currentWords[i].y - currentWords[i].goalY) > WORD_MOVE_MIN_Y_DISTANCE)
		{
			allWordsReachedGoal = false;
		}
		else
		{
			currentWords[i].y = currentWords[i].goalY;
		}
	}

	return allWordsReachedGoal;
}

void ObjWordSearchController::boxReadyCallback()
{
	boxReady = true;
}

void ObjWordSearchController::boxDoneShrinkingCallback()
{
	boxShrunk = true;
}

void ObjWordSearchController::gameClockCompletedCallback()
{
	//TODO
	//Start game over sequence
	state = WordSearchState::WORD_SEARCH_STATE_GAME_OVER;

	wordSearchBox->gameOver();
	
	//Won't do this normally!!!
	//gameManager->setRoomToLoad("titlescreen");
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
		if (std::find(words.begin(), words.end(), currentWords[i].word) != words.end())
		{
			if (currentWords[i].found)
			{
				SDL_Log("ObjWordSearchController: Warning! For some reason a word is being marked as found more than once. Word: %s", currentWords[i].word.c_str());
			}

			currentWords[i].found = true;
		}
	}

	int scoreToAdd;
	float multiplier;
	float mouseX, mouseY;
	int windowWidth, windowHeight;

	//Calculate and add score
	scoreToAdd = 0;
	multiplier = 1;

	for (std::string word : words)
	{
		int wordScore;

		wordScore = getWordLengthScore(word.length());

		scoreToAdd += wordScore;
	}

	multiplier += (words.size() - 1) * WORD_BONUS_MULTIPLIER;

	SDL_Log("ObjWordSearchController: Adding score. Score: %d, Multiplier: %f", scoreToAdd, multiplier);

	SDL_GetMouseState(&mouseX, &mouseY);

	if (!SDL_GetWindowSize(gameManager->getWindow(), &windowWidth, &windowHeight))
	{
		SDL_Log("ObjWordSearchController: Failed to get window size when preparing to create roses. Error: %s", SDL_GetError());

		return;
	}
	
	mouseX = (mouseX / windowWidth) * Config::SCREEN_WIDTH;
	mouseY = (mouseY / windowHeight) * Config::SCREEN_HEIGHT;

	createRoses(mouseX, mouseY, scoreToAdd * multiplier);

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

void ObjWordSearchController::createRoses(double x, double y, float scoreToAdd)
{
	Random *random;
	float multiplier = 1;
	int remainingScoreToAdd;

	random = gameManager->getRandom();

	//Apply difficulty bonus
	multiplier += (difficulty - 1) * DIFFICULTY_MULTIPLIER;
	remainingScoreToAdd = (int) scoreToAdd * multiplier; 

	while (remainingScoreToAdd > 0)
	{
		RoseSize size;
		int nextScore;
		double roseX, roseY;

		if (remainingScoreToAdd >= LARGE_ROSE_SCORE)
		{
			nextScore = LARGE_ROSE_SCORE;
			size = RoseSize::ROSE_SIZE_LARGE;
		}
		else if (remainingScoreToAdd >= MEDIUM_ROSE_SCORE)
		{
			nextScore = MEDIUM_ROSE_SCORE;
			size = RoseSize::ROSE_SIZE_MEDIUM;
		}
		else
		{
			nextScore = SMALL_ROSE_SCORE;
			size = RoseSize::ROSE_SIZE_SMALL;
		}

		remainingScoreToAdd -= nextScore;

		roseX = x + random->getRandomInt(-ROSE_CREATION_RANGE, ROSE_CREATION_RANGE);
		roseY = y + random->getRandomInt(-ROSE_CREATION_RANGE, ROSE_CREATION_RANGE);

		createRose(roseX, roseY, size, nextScore);
	}
}

void ObjWordSearchController::createRose(double x, double y, RoseSize size, int score)
{
	ObjRose *rose;
	double goalX, goalY;
	
	goalX = GUI_TEXT_OFFSET + font->getTextWidth(SCORE_TEXT) / 2.0;
	goalY = scoreYOffset + font->getTextHeight(SCORE_TEXT) / 2.0;

	rose = gameManager->createObject<ObjRose>("objRose", x, y, goalX, goalY, score);
	rose->setRoseSize(size);
	rose->setAddScoreCallback(std::bind(&ObjWordSearchController::addScore, this, std::placeholders::_1));
}

void ObjWordSearchController::addScore(int scoreToAdd)
{
	//TODO
	//Add score over time instead
	
	score += scoreToAdd;
}
