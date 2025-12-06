#ifndef TITLE_SCREEN_H
#define TITLE_SCREEN_H

#include "renderableObject.hpp"

class GameManager;
class BitmapFont;

class ObjTitleScreen : public RenderableObject
{
	public:
		ObjTitleScreen(GameManager *gameManager, double x, double y);

		void renderGui(SDL_Renderer *renderer) override;

		//TODO
		//Callback
		//void startGame():
	private:
		BitmapFont *font;

};

#endif
