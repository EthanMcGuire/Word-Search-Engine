#include "objWordSearchBox.hpp"
#include "bitmapFont.hpp"
#include "assetManager.hpp"
#include "gameManager.hpp"
#include "nineSlice.hpp"
#include "spriteAtlas.hpp"
#include "config.hpp"
#include "random.hpp"
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
	
	state = BoxState::BOX_STATE_WAITING;
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
			//Do nothing. Waiting to be initialized
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
				boxReady();
			}
		}
		break;

		case BoxState::BOX_STATE_READY:
		{
			//Nothing to do. Waiting for one of 2 things to happen:
				//All words are found. Tell controller that the player has won the round
				//Time ran out. Controller will tell the box to stop allowing words to be selected

		}
		break;

		case BoxState::BOX_STATE_GAME_OVER:
		{
			//Waiting to be initialized again
		}
		break;
	}
}

void ObjWordSearchBox::renderGui(SDL_Renderer *renderer)
{
	//Draw letters
	if (state == BoxState::BOX_STATE_READY)
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

void ObjWordSearchBox::setReadyCallback(std::function<void()> callback)
{
	readyCallback = std::move(callback);
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

	state = BoxState::BOX_STATE_SHRINKING;
	showWords = false;
}

void ObjWordSearchBox::gameOver()
{
	state = BoxState::BOX_STATE_GAME_OVER;

	//TODO
	//Disable mouse input
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

		//Add to the grid
		addWordToGrid(word);
	}

	//Scramble the remaining letters
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			if (wordGrid[i][j] == -1)
			{
				grid[i][j] = gameManager->getRandom()->getRandomInt(65, 90);
			}
		}
	}
}

void ObjWordSearchBox::addWordToGrid(std::string word)
{
	int index;
	int length;
	bool success;
	Random *random;

	random = gameManager->getRandom();

	index = currentWords.size();
	length = word.length();
	currentWords.push_back({word, false});	

	success = false;

	while (!success)
	{
		int dir;
		int xDir, yDir;
		int minX, minY, maxX, maxY;
		int wordW, wordH;

		dir = random->getRandomInt(0, 7);
	
		//Direction starts facing right, and goes clockwise:
		//0 - Dog
		//1 - D
		//     o
		//      g
		//2 - D
		//    o
		//    g
		//3 - D
		//   o
		//  g
		//Etc...    
		//
		if (dir == 0)
		{
			wordW = length;
			wordH = 1;
			xDir = 1;
			yDir = 0;

			minX = 0;
			minY = 0;
			maxX = gridSize - wordW;
			maxY = gridSize - wordH;
		}
		else if (dir == 1)
		{
			wordW = length;
			wordH = length;
			xDir = 1;
			yDir = 1;

			minX = 0;
			minY = 0;
			maxX = gridSize - wordW;
			maxY = gridSize - wordH; 
		}
		else if (dir == 2)
		{
			wordW = 1; 
			wordH = length;
			xDir = 0;
			yDir = 1;

			minX = 0;
			minY = 0;
			maxX = gridSize - wordW;
			maxY = gridSize - wordH;
		}
		else if (dir == 3)
		{
			wordW = length;
			wordH = length; 
			xDir = -1;
			yDir = 1;

			minX = wordW - 1; 
			minY = 0;
			maxX = gridSize - 1;
			maxY = gridSize - wordH;
		}
		else if (dir == 4)
		{
			wordW = length;
			wordH = 1;
			xDir = -1;
			yDir = 0;

			minX = wordW - 1; 
			minY = 0;
			maxX = gridSize - 1;
			maxY = gridSize - length;
		}
		else if (dir == 5)
		{
			wordW = length;
			wordH = length;
			xDir = -1;
			yDir = -1;

			minX = wordW - 1;
			minY = wordH - 1;
			maxX = gridSize - 1;
			maxY = gridSize - 1;
		}
		else if (dir == 6)
		{
			wordW = 1;
			wordH = length;
			xDir = 0;
			yDir = -1;

			minX = 0;
			minY = wordH - 1;
			maxX = gridSize - wordW;
			maxY = gridSize - 1;
		}
		else if (dir == 7)
		{
			wordW = length;
			wordH = length; 
			xDir = 1;
			yDir = -1;

			minX = 0;
			minY = wordH - 1;
			maxX = gridSize - length;
			maxY = gridSize - 1;
		}

		for (int i = 0; i < 50; i++)
		{
			int xPos, yPos;
			bool hitWord = false;

			xPos = random->getRandomInt(minX, maxX);
			yPos = random->getRandomInt(minY, maxY);

			//Attempt placement
			//If needed, try to cross another word
			for (int pos = 0; pos < length; pos++)
			{
				int curX, curY;

				curX = xPos + xDir * pos;
				curY = yPos + yDir * pos;

				if (wordGrid[curX][curY] != -1)
				{
					hitWord = true;
					break;

					//TODO
					//Attempt cross here?
				}
			}
			
			if (!hitWord)
			{	
				success = true;
				
				//Place the word in our grid
				for (int pos = 0; pos < length; pos++)
				{
					int curX, curY;

					curX = xPos + xDir * pos;
					curY = yPos + yDir * pos;

					grid[curX][curY] = word[pos];
					wordGrid[curX][curY] = index;
				}

				break;
			}
		}
	}
}

void ObjWordSearchBox::boxReady()
{
	if (readyCallback == NULL)
	{
		throw std::runtime_error("ObjWordSearchBox: Ready callback was not set.");
	}

	readyCallback();
	
	state = BoxState::BOX_STATE_READY;
	showWords = true;
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
