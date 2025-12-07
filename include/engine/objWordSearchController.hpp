#ifndef OBJ_WORD_SEARCH_CONTROLLER_H
#define OBJ_WORD_SEARCH_CONTROLLER_H

#include "renderableObject.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class BitmapFont;
class ObjWordSearchBox;
class ObjGameClock;

struct Word
{
	std::string word;
	bool found = false;
};

class ObjWordSearchController : public RenderableObject
{
	public:
		ObjWordSearchController(GameManager *gameManager, double x, double y);

		void update(double deltaTIme) override;
		void renderGui(SDL_Renderer *renderer) override;

	private:
		void gameClockCompletedCallback();
		void loadWords(std::string path);
		void startNextLevel();
		void getWords();

		std::unordered_map<unsigned int, std::vector<std::string>> words;	//Maps character count to a list of words containing that many characters
		std::vector<Word> currentWords;

		BitmapFont *font;
		ObjWordSearchBox *wordSearchBox;
		ObjGameClock *gameClock;

		const int STARTING_GRID_SIZE = 6;
		const int STARTING_MIN_WORD_COUNT = 3;
		const int STARTING_MAX_WORD_COUNT = 3;
		const int MIN_WORD_LENGTH = 3;	//This will never change. Word length will be MIN_WORD_LENGTH -> GRID_SIZE (Capped at MAX_WORD_LENGTH) 
		const int MAX_WORD_LENGTH = 31;

		int level;
		int score;
		int difficulty;
		int gridSize;
		int wordCountMin;
		int wordCountMax;
};

#endif
