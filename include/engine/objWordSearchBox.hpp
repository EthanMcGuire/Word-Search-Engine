#ifndef OBJ_WORD_SEARCH_BOX_H
#define OBJ_WORD_SEARCH_BOX_H

#include "renderableObject.hpp"
#include "word.hpp"
#include <string>
#include <vector>

class BitmapFont;
class NineSlice;

enum BoxState
{
	BOX_STATE_WAITING,
	BOX_STATE_SHRINKING,
	BOX_STATE_EXPANDING,
	BOX_STATE_READY,
	BOX_STATE_COUNT
};

//Call clearGrid -> enter shrinking state and reset the level shit
//Call populate grid -> Populate grid and words, and start expanding the grid
//Once the grid is expanded, show the words and start

class ObjWordSearchBox : public RenderableObject
{
	public:
		ObjWordSearchBox(GameManager *gameManager, double x, double y);
		~ObjWordSearchBox();

		void update(double deltaTime) override;
		void renderGui(SDL_Renderer *renderer) override;

		void initializeGrid(int size, std::vector<std::string> words);
	private:
		void clearGrid();
		void populateWords(std::vector<std::string> words);

		void updateBoxSize(double deltaTime, int goalSize);
		void setBoxSize(int newSize);

		const int MIN_BOX_SIZE = 32;
		const int DELTA_BOX_SIZE = 256;
		const int LETTER_LEFT_OFFSET = 8;
		const int LETTER_TOP_OFFSET = 8;

		BitmapFont *font;
		NineSlice *box;

		BoxState state = BoxState::BOX_STATE_WAITING;
		int boxSize;
		int boxSizeGoal = 256;
		int gridSize = 5;

		std::vector<Word> currentWords;
		char** grid = NULL;
		int** wordGrid = NULL;	//Maps the locations for our words on the grid
};

#endif
