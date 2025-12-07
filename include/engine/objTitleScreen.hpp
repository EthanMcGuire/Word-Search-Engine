#ifndef TITLE_SCREEN_H
#define TITLE_SCREEN_H

#include "renderableObject.hpp"

class GameManager;
class BitmapFont;

class ObjTitleScreen : public RenderableObject
{
	public:
		ObjTitleScreen(GameManager *gameManager, double x, double y);
		~ObjTitleScreen();

		void renderGui(SDL_Renderer *renderer) override;

	private:
		void mouseCallback(SDL_Event &e);

		BitmapFont *font;
		uint32_t mouseEventListenerId;

};

#endif
