#ifndef OBJ_LEADERBOARD_H
#define OBJ_LEADERBOARD_H

#include "renderableObject.hpp"
#include <string>
#include <vector>

class GameManager;
class BitmapFont;

class ObjLeaderboard : public RenderableObject
{
	public:
		ObjLeaderboard(GameManager *gameManager, double x, double y);
		~ObjLeaderboard();

		void renderGui(SDL_Renderer *renderer) override;

	private:
		void mouseCallback(SDL_Event &e);

		const int BUTTON_TEXT_Y_OFFSET = 32;
		const int LEADERBOARD_Y_OFFSET = 128;
		const int LEADERBOARD_NUMBER_X_OFFSET = 40;
		const int LEADERBOARD_TEXT_X_OFFSET = 192;
		const int LEADERBOARD_TEXT_Y_OFFSET = 64;
		const int LEADERBOARD_TEXT_SEP = 24;

		BitmapFont *font;
		uint32_t mouseEventListenerId;
		std::vector<std::pair<std::string, int>> scoreTable;
};

#endif
