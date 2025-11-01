#include "eventDispatcher.hpp"
#include "event.hpp"
#include <SDL3/SDL_log.h>

/// @brief Adds a listener for the given event type.
/// @tparam T The event type we are listening for (Should derive from Event).
/// @param callback The method to call once the event is dispatched.
/// @return The ID for the added listener. Should be used when we want to remove the listener.
template<typename T>
uint32_t EventDispatcher::addListener(std::function<void(T*)> callback)
{
    static_assert(std::is_base_of<Event, T>::value, "In EventDispatcher::addListener T must derive from Event.");

    std::unordered_set<Uint32> &listenerIdMap = listenerIdMappings[typeid(T)]; 
    uint32_t listenerId;

    listenerId = nextListenerId++;

    listenerIdMap.insert(listenerId);

    std::type_index type = std::type_index(typeid(T));
    std::function<void(Event*)> func = [callback](Event *e) {
        callback(static_cast<T*>(e));
    };

    listeners.emplace(listenerId, std::make_pair(type, func));
    
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EventDispatcher: Added listener with ID = %d", listenerId);

    return listenerId;
}