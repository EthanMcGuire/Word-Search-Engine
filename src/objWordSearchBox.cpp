#include "objWordSearchBox.hpp"
#include "bitmapFont.hpp"
#include "assetManager.hpp"
#include "gameManager.hpp"
#include <stdexcept>

ObjWordSearchBox::ObjWordSearchBox(GameManager *gameManager, double x, double y) : RenderableObject("objWordSearchBox", gameManager, x, y)
{
	font = gameManager->getAssetManager()->getBitmap("fntOpenSans");

	if (font == NULL)
	{
		throw std::runtime_error("ObjWordSearchBox: Failed to load font.");
	}
}

void ObjWordSearchBox::renderGui(SDL_Renderer *renderer)
{
	//font->drawText(renderer, 16, 32, "WORD SEARCH BOX");
}
