#include "objGameClock.hpp"
#include "gameManager.hpp"

ObjGameClock::ObjGameClock(GameManager *gameManager, double x, double y) : RenderableObject("objGameClock", gameManager, x, y)
{
    timer.setCallbackFunction([this](void) {
        this->timerCompleted();
    });
}

/// @brief Updates the Object.
/// @param deltaTime Time change in seconds since last frame.
void ObjGameClock::update(double deltaTime)
{
    timer.update(deltaTime);
}

/// @brief Renders this game objects GUI.
/// @param renderer The renderer to draw to.
void ObjGameClock::renderGui(SDL_Renderer *renderer)
{
    //Draw the clock
	
	//Draw remaining time
	
	//Draw current change time (if not 0)
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
    timer.addTime(addTime);
}

/// @brief Removes time from the clock.
/// @param addTime Time to remove in milliseconds.
void ObjGameClock::removeTime(int removeTime)
{
   timer.removeTime(removeTime); 
}

#pragma endregion Timer