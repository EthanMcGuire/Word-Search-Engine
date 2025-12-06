#pragma once

#ifndef COUNTDOWN_TIMER_H
#define COUNTDOWN_TIMER_H

#include "timer.hpp"
#include <functional>

class CountdownTimer
{
    public:

        CountdownTimer();

        /// @brief Updates the timer.
        /// @param deltaTime Time change between last and current frame.
        void update(double deltaTime);

        /// @brief Sets the callback function to call once the clock finishes counting down.
        /// @param callback The callback function.
        void setCallbackFunction(std::function<void()> callback);

        /// @brief Clears the callback function.
        void clearCallbackFunction();

        #pragma region Timer

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

        /// @return The remaining time in milliseconds.
        int getRemainingTime();

        #pragma endregion Timer

    private:

        const int DELTA_TIME_CHANGE = 30 * 1000;    //1 second for the timer to change by 30 seconds

        Timer timer;
        int totalTime;
        int currentTimeChange;

        bool started;
        bool paused;

        std::function<void()> callback;
};

#endif