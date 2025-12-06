#include "engine.hpp"
#include "eventDispatcher.hpp"
#include <SDL3/SDL_log.h>

/// @brief Adds a listener for the given event type.
/// @tparam T The event type we are listening for (Should derive from Event).
/// @param callback The method to call once the event is dispatched.
template<typename T>
void Engine::addEventListener(std::function<void(T*)> callback)
{
    uint32_t listenerId;

    listenerId = eventDispatcher->addListener<T>(callback);

    eventListenerIds.push(listenerId);
}