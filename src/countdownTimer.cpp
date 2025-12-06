#include "countdownTimer.hpp"
#include <algorithm>

CountdownTimer::CountdownTimer()
{
    totalTime = 0;
    started = false;
    paused = true;

    callback = nullptr;
}

/// @brief Updates the timer.
/// @param deltaTime Time change between last and current frame.
void CountdownTimer::update(double deltaTime)
{
    if (!started || paused)
    {
        return;
    }

    if (currentTimeChange != 0)
	{
		double change;
	
		change = deltaTime * DELTA_TIME_CHANGE;
		
		change = std::min(change, (double) abs(currentTimeChange));
		
		if (currentTimeChange > 0)
		{
			currentTimeChange -= change;
			totalTime += change;
		}
		else
		{
			currentTimeChange += change;
			totalTime -= change;
		}
	}

    if (getRemainingTime() <= 0)
    {
        timer.stop();
        
        started = false;

        if (callback != nullptr)
        {
            callback();
        }
    }
}

/// @brief Sets the callback function to call.
/// @param callback The callback function.
void CountdownTimer::setCallbackFunction(std::function<void()> callback)
{
    this->callback = std::move(callback);
}

/// @brief Clears the callback function.
void CountdownTimer::clearCallbackFunction()
{
    callback = nullptr;
}

#pragma region Timer

/// @brief Starts the countdown timer, resetting it to the total time.
void CountdownTimer::startTimer()
{
    timer.start();

    started = true;
    paused = false;
}

/// @brief Pauses the countdown timer.
void CountdownTimer::pauseTimer()
{
    if (!started) return;

    timer.pause();

    paused = true;
}

/// @brief Unpauses the countdown timer.
void CountdownTimer::unpauseTimer()
{
    if (!started) return;

    timer.unpause();

    paused = false;
}

/// @brief Sets the countdown time.
/// @param time The time in milliseconds.
void CountdownTimer::setTime(int time)
{
    totalTime = time;
}

/// @brief Adds time to the countdown timer.
/// @param addTime Time to add in milliseconds.
void CountdownTimer::addTime(int addTime)
{
    currentTimeChange += addTime;
}

/// @brief Removes time from the countdown timer.
/// @param addTime Time to remove in milliseconds.
void CountdownTimer::removeTime(int removeTime)
{
    currentTimeChange -= removeTime;
}

/// @return The remaining time in milliseconds.
int CountdownTimer::getRemainingTime()
{
    return std::max(totalTime - ((int)timer.getTicks()), 0);
}

#pragma endregion Timer