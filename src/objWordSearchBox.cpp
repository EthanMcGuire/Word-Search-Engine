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
	clearCurrentWords();
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
			if (boxSize == MIN_BOX_SIZE)
			{
				break;
			}

			updateBoxSize(deltaTime, MIN_BOX_SIZE);

			if (boxSize == MIN_BOX_SIZE)
			{
				if (doneShrinkingCallback == NULL)
				{
					throw std::runtime_error("ObjWordSearchBox: doneShrinkingCallback was not set.");
				}

				doneShrinkingCallback();
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
			//Waiting for WordSearchController to tell us the game has started
		}
		break;

		case BoxState::BOX_STATE_ACTIVE:
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
	//Draw box
	if (boxSize > MIN_BOX_SIZE)
	{
		//Draw black alpha background
		SDL_FRect rect;

		rect = {(float) pos[0], (float) pos[1], (float) boxSize, (float) boxSize};

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 96);
		SDL_RenderFillRect(renderer, &rect);

		box->render(renderer, pos[0], pos[1]);	
	}
	
	if (state == BoxState::BOX_STATE_ACTIVE || state == BoxState::BOX_STATE_GAME_OVER)
	{
		drawLetters(renderer);	

		//Draw crossed words
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);

		for (Word* word : currentWords)
		{
			if (word->found)
			{
				float startX, startY;
				float endX, endY;

				startX = grid[word->letterLocations[0].first][word->letterLocations[0].second].x;
				startY = grid[word->letterLocations[0].first][word->letterLocations[0].second].y;
				endX = grid[word->letterLocations.back().first][word->letterLocations.back().second].x;
				endY = grid[word->letterLocations.back().first][word->letterLocations.back().second].y;

				SDL_RenderLine(renderer, startX, startY, endX, endY);
			}
		}
	}
}

void ObjWordSearchBox::drawLetters(SDL_Renderer *renderer)
{
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
	unsigned int surfWidth, surfHeight;
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

	surfWidth = letterInfo.width + 8;
	surfHeight = letterInfo.height + 8;

	letterSurface->createSurface(renderer, surfWidth, surfHeight);
	letterSurface->setBlendMode(SDL_BLENDMODE_BLEND);
	letterSurface->setScaleMode(SDL_SCALEMODE_NEAREST);
	letterSurface->targetSurface(renderer);

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(renderer);

	font->drawTextOutlined(renderer, surfWidth / 2, surfHeight / 2, std::string(1, letterInfo.letter), drawColor, outlineColor, TextAlign::CENTER, TextAlign::CENTER);

	SDL_SetRenderTarget(renderer, NULL);

	letterSurface->drawSurface(renderer, letterInfo.x - (surfWidth * letterScale / 2), letterInfo.y - (surfHeight * letterScale / 2), letterScale, letterScale);
}

void ObjWordSearchBox::setReadyCallback(std::function<void()> callback)
{
	readyCallback = std::move(callback);
}

void ObjWordSearchBox::setDoneShrinkingCallback(std::function<void()> callback)
{
	doneShrinkingCallback = std::move(callback);
}

void ObjWordSearchBox::setWordsFoundCallback(std::function<void(std::vector<std::string>)> callback)
{
	wordsFoundCallback = std::move(callback);
}

void ObjWordSearchBox::setWrongLetterCallback(std::function<void()> callback)
{
	wrongLetterCallback = std::move(callback);
}

void ObjWordSearchBox::initializeGrid(int size, std::vector<std::string> words)
{
	clearGrid();

	gridSize = size;
	boxSizeGoal = boxSizeBase * gridSize;

	grid = new LetterInfo*[gridSize];

	for (int i = 0; i < gridSize; i++)
	{
		grid[i] = new LetterInfo[gridSize];

		for (int j = 0; j < gridSize; j++)
		{
			grid[i][j] = {'?', LetterState::LETTER_STATE_NORMAL, 0, 0, 0, 0};
		}
	}

	populateWords(words);

	state = BoxState::BOX_STATE_EXPANDING;
}

void ObjWordSearchBox::startRound()
{
	if (state != BoxState::BOX_STATE_READY)
	{
		throw std::runtime_error("ObjWordSearchBox: Attempted to start the round and set the box in an active state when we are not ready!");
	}

	roundCompleted = false;
	state = BoxState::BOX_STATE_ACTIVE;
}

void ObjWordSearchBox::completeRound()
{
	roundCompleted = true;
}

void ObjWordSearchBox::shrinkBox()
{
	state = BoxState::BOX_STATE_SHRINKING;
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
		}

		delete[] grid;

		grid = NULL;
	}
}

void ObjWordSearchBox::clearCurrentWords()
{
	for (Word* word : currentWords)
	{
		delete word;
	}

	currentWords.clear();
}

void ObjWordSearchBox::populateWords(std::vector<std::string> words)
{
	if (grid == NULL)
	{
		throw std::runtime_error("ObjWordSearchBox: Attempted to populate words when grid is NULL!");
	}

	clearCurrentWords();

	//Sorts words in descending order. We do this to place the biggest words on the grid first
	std::sort(words.begin(), words.end(), [] (std::string &lhs, std::string &rhs) {
				return lhs.length() > rhs.length();
			});

	for (std::string word : words)
	{
		SDL_Log("ObjWordSearchBox: Adding word: %s", word.c_str());

		//Add to the grid
		addWordToGrid(word);
	}
	
	//Scramble the unset letters
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			if (grid[i][j].letter == '?')
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
	int length;
	bool success;
	Random *random;

	random = gameManager->getRandom();

	length = word.length();

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

				if (grid[curY][curX].letter != '?')
				{
					hitWord = true;
					break;

					//TODO
					//Attempt cross here?
				}
			}
			
			if (!hitWord)
			{	
				Word *newWord;
				std::vector<std::pair<int, int>> letterLocations;

				//Place the word in our grid
				for (int pos = 0; pos < length; pos++)
				{
					int curX, curY;

					curX = xPos + xDir * pos;
					curY = yPos + yDir * pos;

					grid[curY][curX].letter = word[pos];
					grid[curY][curX].width = font->getCharWidth(grid[curY][curX].letter);
					grid[curY][curX].height = font->getCharHeight(grid[curY][curX].letter);

					letterLocations.push_back({curY, curX});
				}

				newWord = new Word({word, false, letterLocations});
				currentWords.push_back(newWord);

				success = true;

				SDL_Log("ObjWordSearchBox: Added word. Word: %s, LetterLocations:", word.c_str());

				for (std::pair<int, int> pair : letterLocations)
				{
					SDL_Log("Y = %d, X = %d", pair.first, pair.second);
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
	if (state != BoxState::BOX_STATE_ACTIVE || roundCompleted) return;
	if (e.button.button != SDL_BUTTON_LEFT) return;
	if (hoveredLetter == NULL) return;

	if (e.button.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
	{
		hoveredLetter->state = LetterState::LETTER_STATE_PRESSED;

		int validWordCount;
		std::vector<Word*> words;

		words = getWordsAtGridLocation(hoveredLetterGridX, hoveredLetterGridY);

		//Remove words that are already found 
		for (int i = words.size() - 1; i >= 0; i--)
		{
			if (words[i]->found)
			{
				SDL_Log("ObjWordSearchBox: Word was already found. Ignoring.");

				words.erase(words.begin() + i);
			}
		}

		validWordCount = words.size();
		
		//At least 1 un-found word as selected?
		if (validWordCount > 0)
		{
			//Success!
			SDL_Log("Correct word found!");

			if (wordsFoundCallback == NULL)
			{
				throw std::runtime_error("ObjWordSearchBox: wordsFoundCallback callback was not set.");
			}

			std::vector<std::string> wordStrings;

			for (Word* word : words)
			{
				wordStrings.push_back(word->word);
				word->found = true;
				
				for (std::pair<int, int> location : word->letterLocations)
				{
					grid[location.first][location.second].state = LetterState::LETTER_STATE_CROSSED;
					grid[location.first][location.second].crossed = true;
				}
			}

			wordsFoundCallback(wordStrings);
		}
		else
		{
			//WRONG ASSHOLE
			SDL_Log("WRONG LETTER ASSHOLE");

			if (wrongLetterCallback == NULL)
			{
				throw std::runtime_error("ObjWordSearchBox: wordsFoundCallback callback was not set.");
			}

			wrongLetterCallback();
		}
	}
	else
	{
		hoveredLetter->state = LetterState::LETTER_STATE_HOVERED;
	}	
}

void ObjWordSearchBox::mouseMoveCallback(SDL_Event &e)
{
	if (state != BoxState::BOX_STATE_ACTIVE || roundCompleted) return;

	SDL_MouseMotionEvent motionEvent;
	LetterInfo *letterInfo = NULL;
	double mouseX, mouseY;
	double mouseXRatio, mouseYRatio;
	int windowWidth, windowHeight;
	int gridX, gridY;

	if (!SDL_GetWindowSize(gameManager->getWindow(), &windowWidth, &windowHeight))
	{
		SDL_Log("ObjWordSearchBox: In mouse move callback, failed to get the window size. Error: %s", SDL_GetError());

		return;
	}
	
	motionEvent = e.motion;
	mouseX = (double) motionEvent.x;
	mouseY = (double) motionEvent.y;

	mouseXRatio = mouseX / windowWidth;
	mouseYRatio = mouseY / windowHeight;

	mouseX = mouseXRatio * Config::SCREEN_WIDTH;
	mouseY = mouseYRatio * Config::SCREEN_HEIGHT;

	//Check if we are over a letter
	for (gridY = 0; gridY < gridSize; gridY++)
	{
		for (gridX = 0; gridX < gridSize; gridX++)
		{
			float left, top, right, bottom;
			unsigned int bboxSize;
			LetterInfo *nextLetterInfo;

			nextLetterInfo = &grid[gridY][gridX];
			bboxSize = SDL_max(nextLetterInfo->width, nextLetterInfo->height);

			left = nextLetterInfo->x - (bboxSize / 2.f);
			top = nextLetterInfo->y - (bboxSize / 2.f);
			right = nextLetterInfo->x + (bboxSize / 2.f);
			bottom = nextLetterInfo->y + (bboxSize / 2.f);

			if (mouseX >= left && mouseX <= right && mouseY >= top && mouseY <= bottom)
			{
				letterInfo = nextLetterInfo;
				break;
			}			
		}

		if (letterInfo != NULL) break;
	}	

	if (letterInfo != NULL)
	{
		if (letterInfo != hoveredLetter)
		{
			clearHoveredLetter();

			hoveredLetter = letterInfo;
			hoveredLetterGridX = gridX;
			hoveredLetterGridY = gridY;
			hoveredLetter->state = LetterState::LETTER_STATE_HOVERED;
		}
	}
	else
	{
		clearHoveredLetter();
	}
}

void ObjWordSearchBox::clearHoveredLetter()
{
	if (hoveredLetter != NULL)
	{
		if (hoveredLetter->crossed)
		{
			hoveredLetter->state = LetterState::LETTER_STATE_CROSSED;
		}
		else
		{
			hoveredLetter->state = LetterState::LETTER_STATE_NORMAL;
		}

		hoveredLetter = NULL;
	}
}

std::vector<Word*> ObjWordSearchBox::getWordsAtGridLocation(int gridX, int gridY)
{
	std::vector<Word*> words;

	SDL_Log("Getting words at location Y: %d, X: %d", gridY, gridX);

	for (Word* word : currentWords)
	{
		for (std::pair<int, int> pair : word->letterLocations)
		{
			if (pair.first == gridY && pair.second == gridX)
			{
				words.push_back(word);

				break;
			}
		}
	}

	return words;
}
