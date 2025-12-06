#include "utility.hpp"
#include <SDL3/SDL_stdinc.h>

template<typename T>
T Utility::lerp(T a, T b, float time)
{
	SDL_clamp(time, 0.0, 1.0);

	return a + (b - a) * time;
}
