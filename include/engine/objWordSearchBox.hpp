#ifndef OBJ_WORD_SEARCH_BOX_H
#define OBJ_WORD_SEARCH_BOX_H

#include "renderableObject.hpp"
#include "word.hpp"
#include <SDL3/SDL_events.h>
#include <string>
#include <vector> 
#include <functional>

class BitmapFont;
class NineSlice;
class DrawingSurface;

enum BoxState
{
	BOX_STATE_WAITING,
	BOX_STATE_SHRINKING,
	BOX_STATE_EXPANDING,
	BOX_STATE_READY,
	BOX_STATE_ACTIVE,
	BOX_STATE_GAME_OVER,
	BOX_STATE_COUNT
};

enum LetterState
{
	LETTER_STATE_NORMAL,
	LETTER_STATE_HOVERED,
	LETTER_STATE_PRESSED,
	LETTER_STATE_CROSSED,
	LETTER_STATE_COUNT
};

struct LetterInfo
{
	char letter;
	LetterState state;
	float x, y;
	unsigned int width, height;
	bool crossed = false;
};

class ObjWordSearchBox : public RenderableObject
{
	public:
		ObjWordSearchBox(GameManager *gameManager, double x, double y);
		~ObjWordSearchBox();

		void update(double deltaTime) override;
		void renderGui(SDL_Renderer *renderer) override;

		void setReadyCallback(std::function<void()> callback);
		void setDoneShrinkingCallback(std::function<void()> callback);
		void setWordsFoundCallback(std::function<void(std::vector<std::string>)> callback);
		void setWrongLetterCallback(std::function<void()> callback);

		void initializeGrid(int size, int letterSep);
		bool addWord(std::string words);
		void allWordsAdded();

		void startRound();
		void completeRound();
		void shrinkBox();
		void gameOver();
	private:
		void drawLetters(SDL_Renderer *renderer);
		void drawLetter(SDL_Renderer *renderer, LetterInfo letterInfo);

		void clearGrid();
		void clearCurrentWords();
		bool addWordToGrid(std::string);

		void boxReady();

		void updateBoxSize(double deltaTime, int goalSize);
		void setBoxSize(int newSize);
		void updateLetterPositions();

		void mouseButtonCallback(SDL_Event &e);
		void mouseMoveCallback(SDL_Event &e);
		void clearHoveredLetter();

		std::vector<Word*> getWordsAtGridLocation(int gridX, int gridY);

		const int MIN_BOX_SIZE = 8;
		const int DELTA_BOX_SIZE = 256;	//Box size change per second
		const int MARGIN_PIXELS = 2;	//Margin in pixels letters will be from the border of the box
		const int MAX_WORD_PLACE_ATTEMPTS = 50;	//Number of times we will attempt to place a word with a given direction
		const int MAX_WORD_ADD_ATTEMPTS = 10;	//Number of times we will attempt to add a word
		const int BOX_MOVE_SPEED = 64;	//Move speed when moving out of the screen at game over

		BitmapFont *font;
		NineSlice *box;
		DrawingSurface *letterSurface;

		//State info
		BoxState state;
		bool roundCompleted = false;

		//Box size variables
		int borderSize;		//The size in pixels of the boxes borders (Nine-slice sprite size)
		int boxSize;
		int boxSizeGoal;
		int boxLetterMargin;	//Inner box margin for letters

		//Current grid data 
		std::vector<Word*> currentWords;
		LetterInfo** grid = NULL;
		int gridSize;

		//Hovered letter info
		LetterInfo *hoveredLetter = NULL;
		int hoveredLetterGridX, hoveredLetterGridY;

		//Callbacks and listeners
		std::function<void()> readyCallback = NULL;
		std::function<void()> doneShrinkingCallback = NULL;
		std::function<void(std::vector<std::string>)> wordsFoundCallback = NULL;
		std::function<void()> wrongLetterCallback = NULL;

		uint32_t mouseButtonDownEventListenerId;
		uint32_t mouseButtonUpEventListenerId;
		uint32_t mouseMoveEventListenerId;
};

#endif
