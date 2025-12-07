#ifndef OBJ_WORD_SEARCH_BOX_H
#define OBJ_WORD_SEARCH_BOX_H

#include "renderableObject.hpp"

class BitmapFont;

class ObjWordSearchBox : public RenderableObject
{
	public:
		ObjWordSearchBox(GameManager *gameManager, double x, double y);

		void renderGui(SDL_Renderer *renderer) override;

	private:
		BitmapFont *font;

};

#endif
