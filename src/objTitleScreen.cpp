#include "objTitleScreen.hpp"
#include "gameManager.hpp"
#include "assetManager.hpp"
#include "bitmapFont.hpp"
#include <stdexcept>

ObjTitleScreen::ObjTitleScreen(GameManager* gameManager, double x, double y) : RenderableObject("objTitleScreen", gameManager, x, y)
{
	font = gameManager->getAssetManager()->getBitmap("fntOpenSans");

	if (font == NULL)
	{
		throw std::runtime_error("ObjTitleScreen: Failed to load font.");
	}
}

void ObjTitleScreen::renderGui(SDL_Renderer *renderer)
{
	font->drawTextAligned(renderer, pos[0], pos[1], "PRESS START", {255, 255, 255, 255}, TextAlign::CENTER, TextAlign::CENTER);
}
