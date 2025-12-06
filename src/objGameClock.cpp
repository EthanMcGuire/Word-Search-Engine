#include "objGameClock.hpp"
#include "gameManager.hpp"
#include "bitmapFont.hpp"
#include "assetManager.hpp"
#include "utility.hpp"
#include "texture.hpp"
#include <stdexcept>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_log.h>

ObjGameClock::ObjGameClock(GameManager *gameManager, double x, double y) : RenderableObject("objGameClock", gameManager, x, y)
{
    font = gameManager->getAssetManager()->getBitmap("fntOpenSans");

    if (font == NULL)
    {
	    throw std::runtime_error("ObjGameClock: Failed to load font.");
    }

    clockTexture = gameManager->getAssetManager()->getTexture("clock");

    if (clockTexture == NULL)
    {
	    throw std::runtime_error("ObjGameClock: Failed to load clock texture.");
    }

    timer.setCallbackFunction([this](void) {
        this->timerCompleted();
    });

    setTime(10000);
    startTimer();

    addTime(15000);
    addTime(-10000);
    addTime(12000);
}

/// @brief Updates the Object.
/// @param deltaTime Time change in seconds since last frame.
void ObjGameClock::update(double deltaTime)
{
    timer.update(deltaTime);

    //Update time changes
    for (int i = timeChanges.size() - 1; i >= 0; i--)
    {
	timeChanges[i].delay = SDL_max(timeChanges[i].delay - (int) (deltaTime * 1000.0), 0);

	if (timeChanges[i].delay == 0)
	{
		timeChanges[i].offsetY = Utility::lerp<double>(timeChanges[i].offsetY, 0.0, TIME_CHANGE_OFFSET_LERP);

		if (timeChanges[i].offsetY <= 2.0)
		{
			timeChanges.erase(timeChanges.begin() + i);
		}
	}
    }
}

/// @brief Renders this game objects GUI.
/// @param renderer The renderer to draw to.
void ObjGameClock::renderGui(SDL_Renderer *renderer)
{
	int remainingTime, currentTimeChange;
	std::string text;

	//Draw the clock
	clockTexture->renderCentered(renderer, pos[0], pos[1]);
	
	//Draw remaining time
	remainingTime = (int) SDL_ceil(timer.getRemainingTime() / 1000.f);

	text = std::to_string(remainingTime);
	font->drawTextAligned(renderer, pos[0] + TIME_OFFSET_X, pos[1], text, {255, 255, 255, 255}, TextAlign::LEFT, TextAlign::CENTER);
	
	//Draw time changes
	for (TimeChange timeChange : timeChanges)
	{
		SDL_Color color;
		int timeSeconds;

		timeSeconds = (int) SDL_ceil(timeChange.time / 1000.f);
		text = std::to_string(timeSeconds);

		if (timeChange.time > 0)
		{
			text = "+" + text;
			color = {0, 255, 0, 255};
		}
		else
		{
			color = {255, 0, 0, 255};
		}

		if (timeChange.offsetY < TIME_CHANGE_OFFSET_Y)
		{
			color.a = (int) (255.0 * (timeChange.offsetY / TIME_CHANGE_OFFSET_Y));
		}

		font->drawTextAligned(renderer, pos[0] + TIME_OFFSET_X, pos[1] + timeChange.offsetY, text, color, TextAlign::LEFT, TextAlign::CENTER);

	}
}

/// @brief Called when the countdown timer finishes. Initiates the game over sequence.
void ObjGameClock::timerCompleted()
{
    //TODO
}

#pragma region Timer

/// @brief Starts the clock, resetting it to the total time.
void ObjGameClock::startTimer()
{
    timer.startTimer();
}

/// @brief Pauses the clock.
void ObjGameClock::pauseTimer()
{
    timer.pauseTimer();
}

/// @brief Unpauses the clock.
void ObjGameClock::unpauseTimer()
{
    timer.unpauseTimer();
}

/// @brief Sets the clock time.
/// @param time The time in milliseconds.
void ObjGameClock::setTime(int time)
{
    timer.setTime(time);
}

/// @brief Adds time to the clock.
/// @param addTime Time to add in milliseconds.
void ObjGameClock::addTime(int addTime)
{
    addTimeChange(addTime); 
    timer.addTime(addTime);
}

/// @brief Removes time from the clock.
/// @param addTime Time to remove in milliseconds.
void ObjGameClock::removeTime(int removeTime)
{
    addTimeChange(-1 * removeTime);
    timer.removeTime(removeTime); 
}

void ObjGameClock::addTimeChange(int time)
{
    double offsetY;

    if (timeChanges.size() == 0)
    {
	    offsetY = TIME_CHANGE_OFFSET_Y;
    }
    else
    {
	    offsetY = timeChanges[timeChanges.size() - 1].offsetY + TIME_CHANGE_OFFSET_Y;
    }

    timeChanges.push_back({offsetY, time, TIME_CHANGE_MOVE_DELAY_MS});
}

#pragma endregion Timer
