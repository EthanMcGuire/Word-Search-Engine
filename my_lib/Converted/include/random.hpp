#pragma once

#ifndef RANDOM_H
#define RANDOM_H

#include <random>

/// @brief Used to generate random numbers.
class Random
{
    public:
        Random();

        /// @brief Gets a random integer in a range (inclusive).
        /// @param min Min value
        /// @param max Max value
        /// @return A random number from min to max (inclusive).
        int getRandomInt(int min, int max);

        /// @brief Gets a random float in a range (inclusive).
        /// @param min Min value
        /// @param max Max value
        /// @return A random number from min to max (inclusive).
        float getRandomFloat(float min, float max);

        /// @brief Gets a random double in a range (inclusive).
        /// @param min Min value
        /// @param max Max value
        /// @return A random number from min to max (inclusive).
        double getRandomDouble(double min, double max);

    private:
        std::random_device rd;
        std::mt19937 rng;
};

#endif