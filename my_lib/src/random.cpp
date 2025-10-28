#include "random.hpp"

Random::Random()
{
    //Seed the random number generator
    rng.seed(rd());
}

/// @brief Gets a random integer in a range (inclusive).
/// @param min Min value
/// @param max Max value
/// @return A random number from min to max (inclusive).
int Random::getRandomInt(int min, int max)
{
    std::uniform_int_distribution<int> dis(min, max);

    return dis(rng);
}

/// @brief Gets a random float in a range (inclusive).
/// @param min Min value
/// @param max Max value
/// @return A random number from min to max (inclusive).
float Random::getRandomFloat(float min, float max)
{
    std::uniform_real_distribution<float> dis(min, max);

    return dis(rng);
}

/// @brief Gets a random double in a range (inclusive).
/// @param min Min value
/// @param max Max value
/// @return A random number from min to max (inclusive).
double Random::getRandomDouble(double min, double max)
{
    std::uniform_real_distribution<double> dis(min, max);

    return dis(rng);
}