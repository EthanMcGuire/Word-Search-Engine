#include "objWordSearchController.hpp"
#include "bitmapFont.hpp"
#include "assetManager.hpp"
#include "gameManager.hpp"
#include "objWordSearchBox.hpp"
#include "objGameClock.hpp"
#include <stdexcept>

ObjWordSearchController::ObjWordSearchController(GameManager *gameManager, double x, double y) : RenderableObject("objWordSearchController", gameManager, x, y)
{
	font = gameManager->getAssetManager()->getBitmap("fntOpenSans");

	if (font == NULL)
	{
		throw std::runtime_error("ObjWordSearchController: Failed to load font.");
	}
}

void ObjWordSearchController::renderGui(SDL_Renderer *renderer)
{
	font->drawText(renderer, 16, 16, "TEST");
}
