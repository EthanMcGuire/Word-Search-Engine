#pragma once

#ifndef TIMER_H
#define TIMER_H

#include <SDL3/SDL_stdinc.h>

class Timer
{
    public:
        Timer();

        /// @brief Starts the timer.
        void start();

        /// @brief Stops and resets the timer.
        void stop();

        /// @brief Pauses the timer.
        void pause();

        /// @brief Resumes the timer.
        void unpause();

        /// @return The number of ticks the timer has been running for.
        Uint32 getTicks() const;

        //Check timer status
        bool isStarted() const;
        bool isPaused() const;

    private:

        //Timer ticks (In Milliseconds)
        Uint32 startTicks;
        Uint32 pausedTicks;

        //Status
        bool paused;
        bool started;
};

#endif