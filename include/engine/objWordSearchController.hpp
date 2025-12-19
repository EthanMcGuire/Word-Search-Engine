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
	//WORD_SEARCH_STATE_START_NEXT_LEVEL,	//Tell box to clear grid and shrink. And give new words. And pause the timer
	WORD_SEARCH_STATE_NEXT_LEVEL_DELAY,
	WORD_SEARCH_STATE_STARTING_ROUND,	//Waiting for the box to fully expand. Will also be sending out the list of words at the same time.
	WORD_SEARCH_STATE_ACTIVE_GAME,		//Timer is active, waiting for the player to find all the words
	WORD_SEARCH_STATE_ROUND_COMPLETED_DELAY,
	WORD_SEARCH_STATE_ENDING_ROUND,		//Words are going away and giving the player score and time. At the same time, the box will begin to shrink.
	WORD_SEARCH_STATE_GAME_OVER,
	WORD_SEARCH_STATE_COUNT
};

struct WordInfo
{
	double x, y;
	double goalY;
	float alpha;
	float goalAlpha;
	std::string word;
	int width, height;
	bool found = false;
};

class ObjWordSearchController : public RenderableObject
{
	public:
		ObjWordSearchController(GameManager *gameManager, double x, double y);

		void update(double deltaTime) override;
		void renderGui(SDL_Renderer *renderer) override;

	private:
		void drawWords(SDL_Renderer *renderer);

		void loadWords(std::string path);
		
		void resetGameData();

		void beginNextLevelDelay();
		/// @brief Starts the next level. Increases difficulty, preps the WordSearchBox, pauses the game clock, and enters us into the STARTING_ROUND state.
		void startNextLevel();
		void getWords();
		void roundCompleted();
		void endRound();

		/// @brief Moves the words to their goal Y locations and adjusts their alphas to their goal alphas.
		/// @return True if all of the words have reached their goal locations.
		bool moveWordsToGoal();

		void boxReadyCallback();
		void boxDoneShrinkingCallback();
		void gameClockCompletedCallback();

		/// @bried Called by the WordSearchBox when a wrong letter is selected.
		void wrongLetterCallback();

		/// @brief Called by the WordSearchBox when one or more words were found.
		/// @param words The list of words that were found.
		void wordsFoundCallback(std::vector<std::string> words);

		int getWordLengthScore(int length);
		void createRoses(double x, double y, float scoreToAdd);
		void createRose(double x, double y, RoseSize size, int score);
		void addScore(int scoreToAdd);

		//Constants
		const int NEXT_LEVEL_DELAY = 500;
		const int END_ROUND_DELAY = 1500;

		//Gui constants
		const int GUI_TEXT_OFFSET = 4;
		const int GUI_TEXT_SEP_Y = 8;
		const int GUI_WORDS_OFFSET_X = 8;
		const int GUI_WORDS_OFFSET_Y = 12;
		const int GUI_WORDS_SEP_Y = 6;

		const float WORD_MOVE_LERP_RATE = 0.2;
		const float WORD_ALPHA_LERP_RATE = 0.2;
		const int WORD_MOVE_MIN_Y_DISTANCE = 1;

		const std::string LEVEL_TEXT = "LEVEL: ";
		const std::string DIF_TEXT = "DIF: ";
		const std::string SCORE_TEXT = "SCORE: ";
		const std::string WORDS_TEXT = "WORDS";
		
		//Time constants
		const int STARTING_TIME = 60000;
		const int WRONG_LETTER_TIME_LOSS = 5000;
		const int CORRECT_WORD_TIME_ADD = 5000;
		const int ROUND_COMPLETED_TIME_ADD = 10000;

		//Difficulty constants
		const int MAX_DIFFICULTY = 20;
		const int DIFFICULTY_INCREASE_ROUND = 5;	//Increase difficulty every 5 rounds

		const int STARTING_GRID_SIZE = 6;
		const int STARTING_MIN_WORD_COUNT = 2;
		const int STARTING_MAX_WORD_COUNT = 3;
		const int MIN_WORD_LENGTH = 3;	//This will never change. Word length will be MIN_WORD_LENGTH -> GRID_SIZE (Capped at MAX_WORD_LENGTH) 
		const int MAX_WORD_LENGTH = 31;

		//Score constants
		const float DIFFICULTY_MULTIPLIER = 0.1;	//Score multiplier added based on difficulty (only if difficulty is above 1)
		const float WORD_BONUS_MULTIPLIER = 0.25;	//Score multiplier added based on the number of words found at once (every extra word is what adds the bonus. So 2 words gets +0.25 multiplier)
		const int ROSE_CREATION_RANGE = 32;
		const int LARGE_ROSE_SCORE = 50;
		const int MEDIUM_ROSE_SCORE = 5;
		const int SMALL_ROSE_SCORE = 1;
		const int WORD_COMPLETED_SCORE_ADD = 100;
		const int SCORE_DELTA_ADD = 500;	//Score to add per second

		std::unordered_map<unsigned int, std::vector<std::string>> words;	//Maps character count to a list of words containing that many characters
		std::vector<bool> wordsFound;
		std::vector<std::pair<int, int>> wordLengthScoreMapping;
		std::vector<WordInfo> currentWords;

		bool boxReady;
		bool boxShrunk;
		int nextLevelDelay;
		int endRoundDelay;

		BitmapFont *font;
		ObjWordSearchBox *wordSearchBox;
		ObjGameClock *gameClock;

		int difYOffset;
		int scoreYOffset;
		int wordsHeaderYOffset;
		int wordsYOffset;

		WordSearchState state;	
		int level;
		int score, scoreAdd;
		int difficulty;
		int gridSize;
		int wordCountMin;
		int wordCountMax;
};

#endif
