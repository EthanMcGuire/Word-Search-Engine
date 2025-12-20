#include "timer.hpp"
#include <SDL3/SDL_timer.h>

Timer::Timer()
{
    startTicks = 0;
    pausedTicks = 0;

    started = false;
    paused = false;
}

/// @brief Starts the timer.
void Timer::start()
{
    started = true;
    paused = false;

    startTicks = SDL_GetTicks();
    pausedTicks = 0;
}

/// @brief Stops and resets the timer.
void Timer::stop()
{
    started = false;
    paused = false;

    startTicks = 0;
    pausedTicks = 0;
}

/// @brief Pauses the timer.
void Timer::pause()
{
    if (started && !paused)
    {
        paused = true;

        pausedTicks = SDL_GetTicks() - startTicks;
        startTicks = 0;
    }
}

/// @brief Resumes the timer.
void Timer::unpause()
{
    if (started && paused)
    {
        paused = false;

        startTicks = SDL_GetTicks() - pausedTicks;
        pausedTicks = 0;
    }
}

/// @return The number of ticks (ms) the timer has been running for.
Uint32 Timer::getTicks() const
{
    Uint32 time = 0;

    //Return the time our timer has been running for
    if (started)
    {
        if (paused)
        {
            time = pausedTicks;
        }
        else
        {
            time = SDL_GetTicks() - startTicks;
        }
    }

    return time;
}

bool Timer::isStarted() const
{
    return started;
}

bool Timer::isPaused() const
{
    return started && paused;
}
