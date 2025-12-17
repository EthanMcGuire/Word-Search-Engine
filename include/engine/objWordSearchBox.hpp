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
		void setWordsFoundCallback(std::function<void(std::vector<std::string>)> callback);
		void setWrongLetterCallback(std::function<void()> callback);

		void shrinkBox();
		void initializeGrid(int size, std::vector<std::string> words);
		void setAsActive();
		void gameOver();
	private:
		void drawLetters(SDL_Renderer *renderer);
		void drawLetter(SDL_Renderer *renderer, LetterInfo letterInfo);

		void clearGrid();
		void clearCurrentWords();
		void populateWords(std::vector<std::string> words);
		void addWordToGrid(std::string);

		void boxReady();

		void updateBoxSize(double deltaTime, int goalSize);
		void setBoxSize(int newSize);
		void updateLetterPositions();

		void mouseButtonCallback(SDL_Event &e);
		void mouseMoveCallback(SDL_Event &e);
		void clearHoveredLetter();

		std::vector<Word*> getWordsAtGridLocation(int gridX, int gridY);

		const int MIN_BOX_SIZE = 32;
		const int DELTA_BOX_SIZE = 256;
		const int BOX_SIZE_BASE = 24;		
		const int BOX_LETTER_MARGIN = 18;

		BitmapFont *font;
		NineSlice *box;
		DrawingSurface *letterSurface;

		BoxState state;
		int boxSize;
		int boxSizeGoal;
		int boxSizeBase;	//Base size in pixels of the box based. Gets multiplied by the number of characters (width/heigth)
		int boxLetterMargin;	//Inner box masrgin for letters
		int gridSize;

		std::vector<Word*> currentWords;
		LetterInfo *hoveredLetter = NULL;
		int hoveredLetterGridX, hoveredLetterGridY;
		LetterInfo** grid = NULL;

		std::function<void()> readyCallback = NULL;
		std::function<void(std::vector<std::string>)> wordsFoundCallback = NULL;
		std::function<void()> wrongLetterCallback = NULL;

		uint32_t mouseButtonDownEventListenerId;
		uint32_t mouseButtonUpEventListenerId;
		uint32_t mouseMoveEventListenerId;
};

#endif
