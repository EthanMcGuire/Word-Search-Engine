#ifndef OBJ_WORD_SEARCH_CONTROLLER_H
#define OBJ_WORD_SEARCH_CONTROLLER_H

#include "renderableObject.hpp"

class BitmapFont;
class ObjWordSearchBox;
class ObjGameClock;

class ObjWordSearchController : public RenderableObject
{
	public:
		ObjWordSearchController(GameManager *gameManager, double x, double y);

		void renderGui(SDL_Renderer *renderer) override;

	private:
		BitmapFont *font;
		ObjWordSearchBox *wordSearchBox;
		ObjGameClock *gameClock;
};

#endif
