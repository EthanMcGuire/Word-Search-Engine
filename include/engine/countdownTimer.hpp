#pragma once

#ifndef COUNTDOWN_TIMER_H
#define COUNTDOWN_TIMER_H

#include "timer.hpp"

//TODO
//Add a callback for when the timer completes!!!

class CountdownTimer
{
    public:

        CountdownTimer();

        /// @brief Updates the timer.
        /// @param deltaTime Time change between last and current frame.
        /// @return True on success, false on failure.
        bool update(double deltaTime);
        void draw();

        /// @brief Starts the countdown timer, resetting it to the total time.
        void startTimer();

        /// @brief Pauses the countdown timer.
        void pauseTimer();

        /// @brief Unpauses the countdown timer.
        void unpauseTimer();

        /// @brief Sets the countdown time.
        /// @param time The time in milliseconds.
        void setTime(int time);

        /// @brief Adds time to the countdown timer.
        /// @param addTime Time to add in milliseconds.
        void addTime(int addTime);

        /// @brief Removes time from the countdown timer.
        /// @param addTime Time to remove in milliseconds.
        void removeTime(int removeTime);

    private:

        /// @return The remaining time in milliseconds.
        int getRemainingTime();

        const int DELTA_TIME_CHANGE = 30 * 1000;    //1 second for the timer to change by 30 seconds

        Timer timer;
        int totalTime;
        int currentTimeChange;

        bool started;
        bool paused;
};

#endif