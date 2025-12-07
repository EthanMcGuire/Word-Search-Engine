#include "objWordSearchBox.hpp"
#include "bitmapFont.hpp"
#include "assetManager.hpp"
#include "gameManager.hpp"
#include "nineSlice.hpp"
#include "spriteAtlas.hpp"
#include <stdexcept>

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
	box->setSize(200, 200);
}

ObjWordSearchBox::~ObjWordSearchBox()
{
	delete box;
}

void ObjWordSearchBox::renderGui(SDL_Renderer *renderer)
{
	//font->drawText(renderer, 16, 32, "WORD SEARCH BOX");
	
	//Draw box
	box->render(renderer, pos[0], pos[1]);	
}
