#include "countdownTimer.hpp"
#include <algorithm>

CountdownTimer::CountdownTimer()
{
    totalTime = 0;
    started = false;
    paused = true;
}

/// @brief Updates the timer.
/// @param deltaTime Time change between last and current frame.
/// @return True on success, false on failure.
bool CountdownTimer::update(double deltaTime)
{
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

    if (started)
    {
        if (getRemainingTime() <= 0)
        {
            timer.stop();
            
            started = false;

            //Timer done!!
            //Call callback
        }
    }

    return true;
}

void CountdownTimer::draw()
{
    //Draw the clock
	
	//Draw remaining time
	
	//Draw current change time (if not 0)
}

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