#include "eventDispatcher.hpp"
#include <SDL3/SDL_log.h>

EventDispatcher::EventDispatcher()
{
    nextListenerId = 0;
    nextSDLListenerId = 0;
}

EventDispatcher::~EventDispatcher()
{
    clearListeners();
}

/// @brief Clears all of the event listeners
void EventDispatcher::clearListeners()
{
    listenerIdMappings.clear();
    listeners.clear();
    sdlListenerIdMappings.clear();
    sdlListeners.clear();

    nextListenerId = 0;
    nextSDLListenerId = 0;  

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EventDispatcher: Finished clearing all listeners.");
}

/// @brief Removes a listener from the given event type.
/// @param id The listener to remove by ID. If the listener does not exist nothing happens.
void EventDispatcher::removeListener(uint32_t id)
{
    if (listeners.find(id) == listeners.end())
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EventDispatcher: Failed to remove listener, does not exist: ID = %d", id);

        return;
    }

    //Remove the Type -> ID mapping if it exists
    auto it = listenerIdMappings.find(listeners.at(id).first);   //First is the event type held in our std::pair

    if (it != listenerIdMappings.end())
    {
        it->second.erase(id);

        if (it->second.empty())
        {
            listenerIdMappings.erase(it);
        }
    }

    listeners.erase(id);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EventDispatcher: Removed listener: ID = %d", id);
}

/// @brief Adds a listener for the given SDL_EventType.
/// @param type The type of SDL_Event to listen for.
/// @param callback The method to call once the event is dispatched.
uint32_t EventDispatcher::addSDLListener(Uint32 type, std::function<void(SDL_Event&)> callback)
{
    std::unordered_set<Uint32> &listenerIdMap = sdlListenerIdMappings[type]; 
    uint32_t listenerId;

    listenerId = nextSDLListenerId++;

    listenerIdMap.insert(listenerId);
    sdlListeners[listenerId] = std::make_pair(type, callback);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EventDispatcher: Added SDL listener: ID = %d", listenerId);

    return listenerId;
}

/// @brief Removes a listener from our SDL listeners.
/// @param id The listener to remove by ID. If the listener does not exist nothing happens.
void EventDispatcher::removeSDLListener(uint32_t id)
{
    if (sdlListeners.find(id) == sdlListeners.end())
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EventDispatcher: Failed to remove SDL listener, does not exist: ID = %d", id);

        return;
    }

    //Remove the Type -> ID mapping if it exists
    auto it = sdlListenerIdMappings.find(sdlListeners.at(id).first);   //First is the event type held in our std::pair

    if (it != sdlListenerIdMappings.end())
    {
        it->second.erase(id);

        if (it->second.empty())
        {
            sdlListenerIdMappings.erase(it);
        }
    }

    sdlListeners.erase(id);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "EventDispatcher: Removed SDL listener with ID = %d", id);
}

/// @brief Dispatches the given event to its listeners.
/// @param event The event that occured.
void EventDispatcher::dispatchEvent(Event *event)
{
    if (event == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "EventDispatcher: Attempted to dispatch a NULL event.");

        return;
    }

    //Get the listener IDs linked to this event
    auto it = listenerIdMappings.find(typeid(*event));

    if (it != listenerIdMappings.end())
    {
        for (auto& id : it->second)
        {
            //Call the linked callback for each ID/listener
            if (listeners.find(id) == listeners.end())
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "EventDispatcher: When dispatching event, listener for ID was not found. ID: %d", id);

                continue;
            }

            listeners.at(id).second(event);
        }
    }
}

/// @brief Dispatches the given SDL event to its listeners.
/// @param event The SDL event that occured.
void EventDispatcher::dispatchEvent(SDL_Event &event)
{
    //Get the listener IDs linked to this event
    auto it = sdlListenerIdMappings.find(event.type);

    if (it != sdlListenerIdMappings.end())
    {
        for (auto& id : it->second)
        {
            //Call the linked callback for each ID/listener
            if (sdlListeners.find(id) == sdlListeners.end())
            {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "EventDispatcher: When dispatching SDL event, listener for ID was not found. SDL_EventType: %d, ID: %d", event.type, id);

                continue;
            }

            sdlListeners.at(id).second(event);
        }
    }
}

/// @brief Removes listeners using the given IDs.
/// @param ids The IDs of listeners to remove.
void EventDispatcher::removeListenersByIds(std::queue<uint32_t> &ids)
{
    while (!ids.empty())
    {
        uint32_t id = ids.front();

        removeListener(id);

        ids.pop();
    }
}

/// @brief Removes SDL listeners using the given IDs.
/// @param ids The IDs of listeners to remove.
void EventDispatcher::removeSDLListenersByIds(std::queue<uint32_t> &ids)
{
    while (!ids.empty())
    {
        uint32_t id = ids.front();

        removeSDLListener(id);

        ids.pop();
    }
}