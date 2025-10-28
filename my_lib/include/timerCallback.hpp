#pragma once

#ifndef TIMER_CALLBACK_H
#define TIMER_CALLBACK_H

#include "timer.hpp"
#include <functional>

class TimerCallback : public Timer
{
    public:

        /// @brief Creates a callback timer.
        /// @param callbackTimeMs Time in Ms to execute the callback at.
        /// @param loop Whether to loop after hitting callback.
        TimerCallback(unsigned int callbackTimeMs = 1000, bool loop = false);

        /// @brief Updates the callback timer, calling the callback function if the time is hit.
        void updateTimer();

        /// @brief Sets the time delay for a callback.
        /// @param callbackTimeMs Time in Ms to execute the callback at.
        void setCallbackTime(unsigned int callbackTimeMs);

        /// @brief Sets whether to loop.
        /// @param loop Whether to loop after hitting callback.
        void setLoop(bool loop);

        /// @brief Sets the callback function to call.
        /// @param callback The callback function.
        void setCallbackFunction(std::function<void()> callback);

        /// @brief Clears the callback function.
        void clearCallbackFunction();

    private:
        unsigned int callbackTimeMs;        //Time at which to call the callback
        bool loop;                          //Whether the timer loops after callback
        std::function<void()> callback;     //Callback function to call
};

#endif