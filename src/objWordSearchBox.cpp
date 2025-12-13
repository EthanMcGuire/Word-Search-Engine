#include "objWordSearchBox.hpp"
#include "bitmapFont.hpp"
#include "assetManager.hpp"
#include "gameManager.hpp"
#include "nineSlice.hpp"
#include "spriteAtlas.hpp"
#include "config.hpp"
#include "random.hpp"
#include "drawingSurface.hpp"
#include "eventDispatcher.hpp"
#include <stdexcept>
#include <algorithm>

ObjWordSearchBox::ObjWordSearchBox(GameManager *gameManager, double x, double y) : RenderableObject("objWordSearchBox", gameManager, x, y)
{
	state = BoxState::BOX_STATE_WAITING;

	font = gameManager->getAssetManager()->getBitmap("fntOpenSans");

	if (font == NULL)
	{
		throw std::runtime_error("ObjWordSearchBox: Failed to load font.");
	}

	//Get the letter sizing and margins based on the font size
	boxSizeBase = font->getCharWidth('W') + 8;
	boxLetterMargin = font->getCharWidth('W') - 2;

	SpriteAtlas *atlas;

	atlas = gameManager->getAssetManager()->getAtlas("wordSearchBox");

	if (atlas == NULL)
	{
		throw std::runtime_error("ObjWordSearchBox: Failed to load atlas for nine slice box.");
	}

	//Create GUI stuff
	box = new NineSlice();
	NineSliceSpriteInfo spriteInfo;

	spriteInfo.atlas = atlas;
	spriteInfo.stretchMiddleCenter = false;	//Repeat the center of the nineslice
	
	box->setSpriteInfo(spriteInfo);

	setBoxSize(MIN_BOX_SIZE);
	
	letterSurface = new DrawingSurface();

	//Input events
	mouseButtonDownEventListenerId = gameManager->getEventDispatcher()->addSDLListener(SDL_EVENT_MOUSE_BUTTON_DOWN, std::bind(&ObjWordSearchBox::mouseButtonCallback, this, std::placeholders::_1));
	mouseButtonUpEventListenerId = gameManager->getEventDispatcher()->addSDLListener(SDL_EVENT_MOUSE_BUTTON_UP, std::bind(&ObjWordSearchBox::mouseButtonCallback, this, std::placeholders::_1));
	mouseMoveEventListenerId = gameManager->getEventDispatcher()->addSDLListener(SDL_EVENT_MOUSE_MOTION, std::bind(&ObjWordSearchBox::mouseMoveCallback, this, std::placeholders::_1));
}

ObjWordSearchBox::~ObjWordSearchBox()
{
	gameManager->getEventDispatcher()->removeSDLListener(mouseButtonDownEventListenerId);
	gameManager->getEventDispatcher()->removeSDLListener(mouseButtonUpEventListenerId);
	gameManager->getEventDispatcher()->removeSDLListener(mouseMoveEventListenerId);

	delete box;
	delete letterSurface;
	
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
	//Draw black alpha background
	SDL_FRect rect;

	rect = {(float) pos[0], (float) pos[1], (float) boxSize, (float) boxSize};

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 96);
	SDL_RenderFillRect(renderer, &rect);
	
	//Draw box
	box->render(renderer, pos[0], pos[1]);	
	
	drawLetters(renderer);	
}

void ObjWordSearchBox::drawLetters(SDL_Renderer *renderer)
{
	if (state != BoxState::BOX_STATE_READY)
	{
		return;
	}

	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			drawLetter(renderer, grid[i][j]);
		}
	}
}

void ObjWordSearchBox::drawLetter(SDL_Renderer *renderer, LetterInfo letterInfo)
{
	SDL_Color drawColor, outlineColor;
	unsigned int letterWidth, letterHeight;
	double letterScale = 1;

	outlineColor = {0, 0, 0, 255};

	switch (letterInfo.state)
	{
		case LetterState::LETTER_STATE_NORMAL:
		{
			drawColor = {255, 255, 255, 255};
		}
		break;

		case LetterState::LETTER_STATE_HOVERED:
		{
			drawColor = {255, 255, 0, 255};
			outlineColor = {128, 0, 128, 255};
		}
		break;

		case LetterState::LETTER_STATE_PRESSED:
		{
			drawColor = {255, 255, 0, 255};
			letterScale = 0.9;
		}
		break;
		
		case LetterState::LETTER_STATE_CROSSED:
		{
			drawColor = {255, 0, 0, 255};
		}
		break;

	}

	letterWidth = letterInfo.width;
	letterHeight = letterInfo.height;

	letterSurface->createSurface(renderer, letterWidth, letterHeight);
	letterSurface->setBlendMode(SDL_BLENDMODE_BLEND);
	letterSurface->setScaleMode(SDL_SCALEMODE_NEAREST);
	letterSurface->targetSurface(renderer);

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);

	font->drawTextOutlined(renderer, letterWidth / 2, letterHeight / 2, std::string(1, letterInfo.letter), drawColor, outlineColor, TextAlign::CENTER, TextAlign::CENTER);

	SDL_SetRenderTarget(renderer, NULL);

	letterSurface->drawSurface(renderer, letterInfo.x - (letterWidth * letterScale / 2), letterInfo.y - (letterHeight * letterScale / 2), letterScale, letterScale);
}

void ObjWordSearchBox::setReadyCallback(std::function<void()> callback)
{
	readyCallback = std::move(callback);
}

void ObjWordSearchBox::initializeGrid(int size, std::vector<std::string> words)
{
	clearGrid();

	gridSize = size;
	boxSizeGoal = boxSizeBase * gridSize;

	grid = new LetterInfo*[gridSize];
	wordGrid = new int*[gridSize];

	for (int i = 0; i < gridSize; i++)
	{
		grid[i] = new LetterInfo[gridSize];
		wordGrid[i] = new int[gridSize];

		for (int j = 0; j < gridSize; j++)
		{
			grid[i][j] = {'A', LetterState::LETTER_STATE_NORMAL, 0, 0, 0, 0};
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
	clearHoveredLetter();

	//TODO
	//Disable mouse input
}

void ObjWordSearchBox::clearGrid()
{
	if (grid != NULL)
	{
		clearHoveredLetter();

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
	
	//Scramble the unset letters
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			if (wordGrid[i][j] == -1)
			{
				grid[i][j].letter = gameManager->getRandom()->getRandomInt(65, 90);
				grid[i][j].width = font->getCharWidth(grid[i][j].letter);
				grid[i][j].height = font->getCharHeight(grid[i][j].letter);
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

		SDL_Log("ObjWordSearchBox: Attempting to add word with dir %d...", dir);
	
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
			maxY = gridSize - wordH;
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

					grid[curX][curY].letter = word[pos];
					grid[curX][curY].width = font->getCharWidth(grid[curX][curY].letter);
					grid[curX][curY].height = font->getCharHeight(grid[curX][curY].letter);

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

	updateLetterPositions();
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

	updateLetterPositions();
}

void ObjWordSearchBox::updateLetterPositions()
{
	if (grid == NULL) return;

	int letterX, letterY;
	int availableSpace;
	int letterSpacing;

	availableSpace = boxSize - (boxLetterMargin * 2);
	letterSpacing = availableSpace / (gridSize - 1);

	letterY = (int) (pos[1] + boxLetterMargin);

	for (int i = 0; i < gridSize; i++)
	{
		letterX = (int) (pos[0] + boxLetterMargin);

		for (int j = 0; j < gridSize; j++)
		{
			grid[i][j].x = letterX;
			grid[i][j].y = letterY;

			letterX += letterSpacing;
		}

		letterY += letterSpacing;
	}
}

void ObjWordSearchBox::mouseButtonCallback(SDL_Event &e)
{
	if (e.button.button != SDL_BUTTON_LEFT) return;
	if (hoveredLetter == NULL) return;

	if (e.button.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
	{
		hoveredLetter->state = LetterState::LETTER_STATE_PRESSED;
	}
	else
	{
		hoveredLetter->state = LetterState::LETTER_STATE_HOVERED;
	}	
}

void ObjWordSearchBox::mouseMoveCallback(SDL_Event &e)
{
	if (state != BoxState::BOX_STATE_READY) return;

	SDL_MouseMotionEvent motionEvent;
	float mouseX, mouseY;
	LetterInfo *letterInfo;

	motionEvent = e.motion;
	mouseX = motionEvent.x;
	mouseY = motionEvent.y;

	clearHoveredLetter();

	//Check if we are over a letter
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			float left, top, right, bottom;
			unsigned int bboxSize;
			LetterInfo *nextLetterInfo;

			nextLetterInfo = &grid[i][j];
			bboxSize = SDL_max(nextLetterInfo->width, nextLetterInfo->height);

			left = nextLetterInfo->x - (bboxSize / 2.f);
			top = nextLetterInfo->y - (bboxSize / 2.f);
			right = nextLetterInfo->x + (bboxSize / 2.f);
			bottom = nextLetterInfo->y + (bboxSize / 2.f);

			if (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom)
			{
				hoveredLetter = nextLetterInfo;
				hoveredLetter->state = LetterState::LETTER_STATE_HOVERED;

				break;
			}			
		}
	}	
}

void ObjWordSearchBox::clearHoveredLetter()
{
	if (hoveredLetter != NULL)
	{
		hoveredLetter->state = LetterState::LETTER_STATE_NORMAL;
		hoveredLetter = NULL;
	}
}
