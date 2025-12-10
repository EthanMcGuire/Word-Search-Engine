#include "objWordSearchBox.hpp"
#include "bitmapFont.hpp"
#include "assetManager.hpp"
#include "gameManager.hpp"
#include "nineSlice.hpp"
#include "spriteAtlas.hpp"
#include "config.hpp"
#include <stdexcept>
#include <algorithm>

ObjWordSearchBox::ObjWordSearchBox(GameManager *gameManager, double x, double y) : RenderableObject("objWordSearchBox", gameManager, x, y)
{
	font = gameManager->getAssetManager()->getBitmap("fntOpenSans");

	if (font == NULL)
	{
		throw std::runtime_error("ObjWordSearchBox: Failed to load font.");
	}

	SpriteAtlas *atlas;

	atlas = gameManager->getAssetManager()->getAtlas("wordSearchBox");

	if (atlas == NULL)
	{
		throw std::runtime_error("ObjWordSearchBox: Failed to load atlas for nine slice box.");
	}

	box = new NineSlice();
	box->setSprites(atlas, 0, 1, 2, 3, 4, 5, 6, 7, 8);

	setBoxSize(MIN_BOX_SIZE);
}

ObjWordSearchBox::~ObjWordSearchBox()
{
	delete box;
	
	clearGrid();
}

void ObjWordSearchBox::update(double deltaTime)
{
	switch (state)
	{
		case BoxState::BOX_STATE_WAITING:
		{

		}
		break;

		case BoxState::BOX_STATE_SHRINKING:
		{
			updateBoxSize(deltaTime, MIN_BOX_SIZE);

			if (boxSize == MIN_BOX_SIZE)
			{
				state = BoxState::BOX_STATE_EXPANDING;
			}
		}
		break;

		case BoxState::BOX_STATE_EXPANDING:
		{
			updateBoxSize(deltaTime, boxSizeGoal);

			if (boxSize == boxSizeGoal)
			{
				state = BoxState::BOX_STATE_READY;
			}
		}
		break;

		case BoxState::BOX_STATE_READY:
		{

		}
		break;
	}
}

void ObjWordSearchBox::renderGui(SDL_Renderer *renderer)
{
	//Draw letters
	if (grid != NULL)
	{
		int drawX, drawY;
		int letterHorizontalSpacing = 16;
		int letterVerticalSpacing = 16;

		drawY = (int) (pos[1] + LETTER_TOP_OFFSET);

		for (int i = 0; i < gridSize; i++)
		{
			drawX = (int) (pos[0] + LETTER_LEFT_OFFSET);

			for (int j = 0; j < gridSize; j++)
			{
				font->drawText(renderer, drawX, drawY, std::string(1, grid[i][j]));

				drawX += letterHorizontalSpacing;
			}

			drawY += letterVerticalSpacing;
		}
	}
	
	//Draw box
	box->render(renderer, pos[0], pos[1]);	
}

void ObjWordSearchBox::initializeGrid(int size, std::vector<std::string> words)
{
	clearGrid();

	gridSize = size;

	grid = new char*[gridSize];
	wordGrid = new int*[gridSize];

	for (int i = 0; i < gridSize; i++)
	{
		grid[i] = new char[gridSize];
		wordGrid[i] = new int[gridSize];

		for (int j = 0; j < gridSize; j++)
		{
			grid[i][j] = 'A';
			wordGrid[i][j] = -1;
		}
	}

	populateWords(words);
}

void ObjWordSearchBox::clearGrid()
{
	if (grid != NULL)
	{
		for (int i = 0; i < gridSize; i++)
		{
			delete[] grid[i];
			delete[] wordGrid[i];
		}

		delete[] grid;
		delete[] wordGrid;

		grid = NULL;
		wordGrid = NULL;
	}
}

void ObjWordSearchBox::populateWords(std::vector<std::string> words)
{
	if (grid == NULL)
	{
		throw std::runtime_error("ObjWordSearchBox: Attempted to populate words when grid is NULL!");
	}

	currentWords.clear();

	//Sorts words in descending order. We do this to place the biggest words on the grid first
	std::sort(words.begin(), words.end(), [] (std::string &lhs, std::string &rhs) {
				return lhs.length() > rhs.length();
			});

	for (std::string word : words)
	{
		SDL_Log("ObjWordSearchBox: Added word: %s", word.c_str());
		currentWords.push_back({word, false});
	}

	//Fill the grid
}

void ObjWordSearchBox::updateBoxSize(double deltaTime, int goalSize)
{
	int newSize;

	if (goalSize > boxSize)
	{
		newSize = SDL_min(boxSize + (DELTA_BOX_SIZE * deltaTime), goalSize);
	}
	else
	{
		newSize = SDL_max(boxSize - (DELTA_BOX_SIZE * deltaTime), goalSize);
	}

	setBoxSize(newSize);
}

void ObjWordSearchBox::setBoxSize(int newSize)
{
	boxSize = newSize;

	box->setSize(boxSize, boxSize);
	pos = {(double) Config::SCREEN_WIDTH / 2 - boxSize / 2, (double) Config::SCREEN_HEIGHT / 2 - boxSize / 2};
}
