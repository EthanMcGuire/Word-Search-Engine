#ifndef OBJ_WORD_SEARCH_CONTROLLER_H
#define OBJ_WORD_SEARCH_CONTROLLER_H

#include "renderableObject.hpp"
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

		const int STARTING_TIME = 60000;
		const int STARTING_GRID_SIZE = 6;
		const int STARTING_MIN_WORD_COUNT = 2;
		const int STARTING_MAX_WORD_COUNT = 3;
		const int MIN_WORD_LENGTH = 3;	//This will never change. Word length will be MIN_WORD_LENGTH -> GRID_SIZE (Capped at MAX_WORD_LENGTH) 
		const int MAX_WORD_LENGTH = 31;

		std::unordered_map<unsigned int, std::vector<std::string>> words;	//Maps character count to a list of words containing that many characters
		std::vector<std::string> currentWords;

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
