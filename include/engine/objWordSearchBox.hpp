#ifndef OBJ_WORD_SEARCH_BOX_H
#define OBJ_WORD_SEARCH_BOX_H

#include "renderableObject.hpp"

class BitmapFont;
class NineSlice;

class ObjWordSearchBox : public RenderableObject
{
	public:
		ObjWordSearchBox(GameManager *gameManager, double x, double y);
		~ObjWordSearchBox();

		void renderGui(SDL_Renderer *renderer) override;

	private:
		BitmapFont *font;
		NineSlice *box;
};

#endif
