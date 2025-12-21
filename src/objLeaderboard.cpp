#include "objLeaderboard.hpp"
#include "gameManager.hpp"
#include "assetManager.hpp"
#include "bitmapFont.hpp"
#include "eventDispatcher.hpp"
#include "config.hpp"
#include "scoreTable.hpp"
#include <stdexcept>

ObjLeaderboard::ObjLeaderboard(GameManager* gameManager, double x, double y) : RenderableObject("objLeaderboard", gameManager, x, y)
{
	//font = gameManager->getAssetManager()->getBitmap("newsGothic");
	font = gameManager->getAssetManager()->getBitmap("sitka");

	if (font == NULL)
	{
		throw std::runtime_error("ObjLeaderboard: Failed to load font.");
	}

	mouseEventListenerId = gameManager->getEventDispatcher()->addSDLListener(SDL_EVENT_MOUSE_BUTTON_DOWN, [this](SDL_Event &e) {
		this->mouseCallback(e);
	});
}

ObjLeaderboard::~ObjLeaderboard()
{
	gameManager->getEventDispatcher()->removeSDLListener(mouseEventListenerId);
}

void ObjLeaderboard::renderGui(SDL_Renderer *renderer)
{
	int drawX, drawY;
	int textW, textH;
	std::string text;
	std::vector<std::pair<std::string, int>> scoreTable;

	drawX = Config::SCREEN_WIDTH / 2;
	drawY = BUTTON_TEXT_Y_OFFSET;

	font->drawTextOutlined(renderer, drawX, drawY, "PRESS LEFT MOUSE BUTTON", {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::CENTER, TextAlign::CENTER);

	drawY = LEADERBOARD_Y_OFFSET;
	text = "LEADERBOARD";
	textW = font->getTextWidth(text);
	textH = font->getTextHeight(text);

	font->drawTextOutlined(renderer, drawX, drawY, text, {130, 200, 229, 255}, {0, 0, 0, 255}, TextAlign::CENTER, TextAlign::CENTER);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderLine(renderer, drawX - textW / 2, drawY + 2 + textH / 2, drawX + textW / 2, drawY + 2 + textH / 2);

	scoreTable = ScoreTable::readPlayerScoreTable();
	std::sort(scoreTable.begin(), scoreTable.end(), [](const std::pair<std::string, int> &a, const std::pair<std::string, int> &b) {
			return a.second > b.second;	
		});

	drawY = LEADERBOARD_Y_OFFSET + LEADERBOARD_TEXT_Y_OFFSET;

	for (int i = 0; i < scoreTable.size() && i < 10; i++)
	{
		drawX = Config::SCREEN_WIDTH / 2 - LEADERBOARD_TEXT_X_OFFSET;
		font->drawTextOutlined(renderer, drawX - LEADERBOARD_NUMBER_X_OFFSET, drawY, std::to_string(i + 1) + ".", {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::CENTER);
		font->drawTextOutlined(renderer, drawX, drawY, scoreTable[i].first, {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::LEFT, TextAlign::CENTER);

		drawX = Config::SCREEN_WIDTH / 2 + LEADERBOARD_TEXT_X_OFFSET;
		font->drawTextOutlined(renderer, drawX, drawY, std::to_string(scoreTable[i].second), {255, 255, 255, 255}, {0, 0, 0, 255}, TextAlign::RIGHT, TextAlign::CENTER);

		drawY += LEADERBOARD_TEXT_SEP;
	}
}

void ObjLeaderboard::mouseCallback(SDL_Event &e)
{
	if (e.button.button == SDL_BUTTON_LEFT)
	{
		gameManager->setRoomToLoad("titlescreen");
	}
}
