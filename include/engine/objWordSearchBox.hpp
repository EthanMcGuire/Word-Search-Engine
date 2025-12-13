#ifndef OBJ_WORD_SEARCH_BOX_H
#define OBJ_WORD_SEARCH_BOX_H

#include "renderableObject.hpp"
#include "word.hpp"
#include <string>
#include <vector>
#include <functional>

class BitmapFont;
class NineSlice;

enum BoxState
{
	BOX_STATE_WAITING,
	BOX_STATE_SHRINKING,
	BOX_STATE_EXPANDING,
	BOX_STATE_READY,
	BOX_STATE_GAME_OVER,
	BOX_STATE_COUNT
};

class ObjWordSearchBox : public RenderableObject
{
	public:
		ObjWordSearchBox(GameManager *gameManager, double x, double y);
		~ObjWordSearchBox();

		void update(double deltaTime) override;
		void renderGui(SDL_Renderer *renderer) override;

		void setReadyCallback(std::function<void()> callback);

		void initializeGrid(int size, std::vector<std::string> words);
		void gameOver();
	private:
		void clearGrid();
		void populateWords(std::vector<std::string> words);
		void addWordToGrid(std::string);

		void boxReady();

		void updateBoxSize(double deltaTime, int goalSize);
		void setBoxSize(int newSize);

		const int MIN_BOX_SIZE = 32;
		const int DELTA_BOX_SIZE = 256;
		const int BOX_SIZE_BASE = 24;		
		const int BOX_LETTER_MARGIN = 18;

		BitmapFont *font;
		NineSlice *box;

		BoxState state;
		int boxSize;
		int boxSizeGoal;
		int boxSizeBase;	//Base size in pixels of the box based. Gets multiplied by the number of characters (width/heigth)
		int boxLetterMargin;	//Inner box masrgin for letters
		int gridSize;

		std::vector<Word> currentWords;
		char** grid = NULL;
		int** wordGrid = NULL;	//Maps the locations for our words on the grid
		bool showWords = false;

		std::function<void()> readyCallback;
};

#endif
