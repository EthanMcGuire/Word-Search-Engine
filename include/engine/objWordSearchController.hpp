#ifndef OBJ_WORD_SEARCH_CONTROLLER_H
#define OBJ_WORD_SEARCH_CONTROLLER_H

#include "renderableObject.hpp"
#include "objRose.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class BitmapFont;
class ObjWordSearchBox;
class ObjGameClock;

enum WordSearchState
{
	//WORD_SEARCH_STATE_START_NEXT_LEVEL,	//Tell box to clear grid and shrink. And give new words. And pause the timer. AND create level text instance
	WORD_SEARCH_STATE_WAITING_FOR_BOX,	//Waiting for the box to shrink, then expand
	WORD_SEARCH_STATE_ACTIVE_GAME,		//Timer is active, waiting for the player to find all the words
	WORD_SEARCH_STATE_GAME_OVER,
	WORD_SEARCH_STATE_COUNT
};

class ObjWordSearchController : public RenderableObject
{
	public:
		ObjWordSearchController(GameManager *gameManager, double x, double y);

		void update(double deltaTime) override;
		void renderGui(SDL_Renderer *renderer) override;

	private:
		void resetGameData();

		void loadWords(std::string path);
		
		void startNextLevel();
		void getWords();

		void boxReadyCallback();
		void gameClockCompletedCallback();

		/// @bried Called by the WordSearchBox when a wrong letter is selected.
		void wrongLetterCallback();

		/// @brief Called by the WordSearchBox when one or more words were found.
		/// @param words The list of words that were found.
		void wordsFoundCallback(std::vector<std::string> words);

		int getWordLengthScore(int length);
		void createRoses(double x, double y, int scoreToAdd);
		void createRose(double x, double y, RoseSize size, int score);
		void addScore(int scoreToAdd);

		const int GUI_TEXT_OFFSET = 4;
		const int GUI_WORD_SEP_Y = 8;
		const int GUI_WORDS_OFFSET_X = 8;
		const int GUI_WORDS_OFFSET_Y = 12;
		const int STARTING_TIME = 60000;
		const int WRONG_LETTER_TIME_LOSS = 5000;
		const int CORRECT_WORD_TIME_ADD = 5000;
		const int STARTING_GRID_SIZE = 6;
		const int STARTING_MIN_WORD_COUNT = 2;
		const int STARTING_MAX_WORD_COUNT = 3;
		const int MIN_WORD_LENGTH = 3;	//This will never change. Word length will be MIN_WORD_LENGTH -> GRID_SIZE (Capped at MAX_WORD_LENGTH) 
		const int MAX_WORD_LENGTH = 31;
		const int MAX_DIFFICULTY = 20;
		const float DIFFICULTY_MULTIPLIER = 0.1;	//Score multiplier added based on difficulty (only if difficulty is above 1)
		const float WORD_BONUS_MULTIPLIER = 0.25;	//Score multiplier added based on the number of words (every extra word is what adds the bonus. So 2 words gets +0.25 multiplier)
		const int ROSE_CREATION_RANGE = 32;

		std::unordered_map<unsigned int, std::vector<std::string>> words;	//Maps character count to a list of words containing that many characters
		std::vector<std::string> currentWords;
		std::vector<bool> wordsFound;
		std::vector<std::pair<int, int>> wordLengthScoreMapping;

		BitmapFont *font;
		ObjWordSearchBox *wordSearchBox;
		ObjGameClock *gameClock;

		WordSearchState state;	
		int level;
		int score;
		int difficulty;
		int gridSize;
		int wordCountMin;
		int wordCountMax;
};

#endif
