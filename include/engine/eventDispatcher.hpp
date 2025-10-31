#pragma once

#ifndef EVENT_DISPATCHER_H
#define EVENT_DISPATCHER_H

#include "event.hpp"
#include "SDL3/SDL_events.h"
#include <functional>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>
#include <queue>
#include <utility>

/// @brief RETARD ALERT
///        YOU CAN CALL addListener() TWICE FOR THE SAME FUNCTION!!!! BE CAUTIOUS!!!
///        IF YOU ADD IT TWICE AND FORGOT TO REMOVE IT THE FIRST TIME, YOU NOW HAVE AN EXTRA POINTER TO THE SAME FUNCTION!!! If you don't have both listener ID's, you will NOT be able to remove the function reference!!
///
///         Other Info:
///
///             -Callbacks are called in the order they were added for each event type, since we are using unordered_maps.
class EventDispatcher
{
    public:
        EventDispatcher();
        ~EventDispatcher();
        
        /// @brief Clears all of the event listeners
        void clearListeners();

        /// @brief Adds a listener for the given event type.
        /// @tparam T The event type we are listening for (Should derive from Event).
        /// @param callback The method to call once the event is dispatched.
        /// @return The ID for the added listener. Should be used when we want to remove the listener.
        template<typename T>
        uint32_t addListener(std::function<void(T*)> callback);

        /// @brief Removes a listener from our listeners.
        /// @param id The listener to remove by ID. If the listener does not exist nothing happens.
        void removeListener(uint32_t id);

        /// @brief Adds a listener for the given SDL_EventType.
        /// @param type The type of SDL_Event to listen for.
        /// @param callback The method to call once the event is dispatched.
        /// @return The ID for the added listener. Should be used when we want to remove the listener.
        uint32_t addSDLListener(Uint32 type, std::function<void(SDL_Event&)> callback);

        /// @brief Removes a listener from our SDL listeners.
        /// @param id The listener to remove by ID. If the listener does not exist nothing happens.
        void removeSDLListener(uint32_t id);

        /// @brief Dispatches the given event to its listeners.
        /// @param event The event that occured.
        void dispatchEvent(Event *event);

        /// @brief Dispatches the given SDL event to its listeners.
        /// @param event The SDL event that occured.
        void dispatchEvent(SDL_Event &event);

        /// @brief Removes listeners using the given IDs.
        /// @param ids The IDs of listeners to remove.
        void removeListenersByIds(std::queue<uint32_t> &ids);

        /// @brief Removes SDL listeners using the given IDs.
        /// @param ids The IDs of listeners to remove.
        void removeSDLListenersByIds(std::queue<uint32_t> &ids);
    
    private:
        std::unordered_map<std::type_index, std::unordered_set<Uint32>> listenerIdMappings;                 //Links event type to listener IDs
        std::unordered_map<Uint32, std::pair<std::type_index, std::function<void(Event*)>> > listeners;     //Links listener ID to event type and callback

        std::unordered_map<Uint32, std::unordered_set<Uint32>> sdlListenerIdMappings;                       //Links event type to listener IDs
        std::unordered_map< Uint32, std::pair<Uint32, std::function<void(SDL_Event&)>> > sdlListeners;      //Links listener ID to event type and callback

        uint32_t nextListenerId;
        uint32_t nextSDLListenerId;
};

#include "eventDispatcher.tpp"

#endif