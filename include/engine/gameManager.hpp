#pragma once

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <SDL3/SDL_events.h>
#include <queue>
#include <string>
#include <vector>

#include "collision.hpp"
#include "parameterVariant.hpp"

class Engine;
class Camera;
class Room;
class Random;
class ObjectManager;
class AudioController;
class AssetManager;
class EventDispatcher;
class SDL_Renderer;
class BitmapFont;
class Object;

class GameManager
{
    public:

        GameManager(Engine *engine);
        ~GameManager();
        
        /// @brief Initializes the game manager by creating the room and camera.
        void initializeGameManager();

        #pragma region GameControl

        /// @brief Ends the game.
        void endGame() const;

        /// @brief Pauses the game.
        void pauseGame();

        /// @brief Unpauses the game.
        void unpauseGame();

        #pragma endregion GameControl

        #pragma region Game_Loop

        /// @brief Updates the game.
        /// @param deltaTime Time change between last and current frame.
        /// @return True on success, false on failure.
        bool update(double deltaTime);

        /// @brief Draws the room background and the game objects.
        /// @param renderer The game windows renderer.
        void draw(SDL_Renderer *renderer);

        /// @brief Draws the game GUI. This includes the HUD and any debug information.
        /// @param renderer The game windows renderer.
        void drawGui(SDL_Renderer *renderer);

        #pragma endregion Game_Loop

        #pragma region Game_State

        /// @brief Sets a room to load at the end of the frame.
        /// @param nextRoomToLoad The name of the room to load.
        void setRoomToLoad(std::string nextRoomToLoad);
        
        #pragma endregion Game_State

        #pragma region Game_Objects

        /// @brief Creates a game object from the ObjectFactory, Adding the object to the ObjectManager.
        ///        For Menu's, the playerIndex will be set to 0. To set the playerIndex, call createMenuObject() instead.
        /// @tparam ...Args Extra arg types.
        /// @tparam T Object class to return. MUST derive from Object.
        /// @param objectName The name of the object to create.
        /// @param x X position.
        /// @param y Y position.
        /// @param ...args Extra args
        /// @return The object reference.
        template <typename T = Object, typename... Args>
        T* createObject(std::string objectName, double x, double y, Args... args);

        /// @brief Creates a game object from the ObjectFactory, Adding the object to the ObjectManager.
        ///        For Menu's, the playerIndex will be set to 0. To set the playerIndex, call createMenuObject() instead.
        /// @tparam ...Args Extra arg types.
        /// @tparam T Object class to return. MUST derive from Object.
        /// @param objectName The name of the object to create.
        /// @param x X position.
        /// @param y Y position.
        /// @param ...args Extra args
        /// @return The object reference.
        template <typename T = Object, typename... Args>
        T* createObject(std::string objectName, double x, double y, std::vector<ParameterVariant> args);

        /// @brief Destroys a game object.
        /// @param object The object to destroy.
        void destroyGameObject(Object *object);

        /// @brief Destroys a game object.
        /// @param id ID of the object to destroy.
        void destroyGameObject(int id);

        /// @brief Destroys the game objects that are pending to be destroyed. Should be called at the end of each frame.
        void destroyGameObjects();

        /// @brief Gets the next unique ID from the object manager. Used to uniquely identify each object.
        /// @return The next ID.
        unsigned int getNextObjectId();

        /// @tparam T The type of game object to check.
        /// @return True if the game object exists. False otherwise
        template <typename T> bool gameObjectExists() const;

        #pragma endregion Game_Objects

        #pragma region Collision

        /// @brief Checks if a collision is meeting with a specific game object type.
        /// @tparam T The game object type to check.
        /// @param objectBBox The bounding box.
        /// @return True on collision, false otherwise.
        template <typename T> bool placeMeetingGameObject(BBox objectBBox) const;

        /// @brief Checks for a collision with a object type, and returns the first found instance if so.
        /// @tparam T The game object type to check.
        /// @param objectBBox The bounding box.
        /// @return The Object on collision, NULL otherwise.
        template <typename T> T* getInstancePlace(BBox objectBBox) const;

        /// @brief Checks for and returns all meeting instances of an object type.
        /// @tparam T The game object type to check.
        /// @param objectBBox The bounding box.
        /// @return The list of meeting game objects.
        template <typename T> std::vector<T*> getInstancePlaceList(BBox objectBBox) const;

        #pragma endregion Collision

        #pragma region Getters

        /// @return True if the game is paused, false otherwise.
        bool getGamePaused();

        /// @return The game camera.
        Camera* getCamera() const;

        /// @return The current room.
        Room* getRoom() const;

        /// @return The Random number generator class.
        Random* getRandom() const;

        /// @return The asset manager. Used to pull various assets.
        AssetManager* getAssetManager() const;

        /// @return The audio controller. Used to play music and sounds.
        AudioController* getAudioController() const;

        /// @return The EventDispatcher. Used to add event listeners and dispatch events.
        EventDispatcher* getEventDispatcher() const;

        /// @brief Pulls a bitmap font from the asset manager.
        /// @param name The font name.
        /// @return The font, or nullptr.
        BitmapFont* getBitmapFont(std::string name) const;

        /*
        /// @brief Pulls an animation from the animation manager.
        /// @param name The animation name.
        /// @return The animation info, or NULL.
        AnimationInfo* getAnimation(std::string name) const;
        */

        #pragma endregion Getters

    private:
        /// @brief Object's need to be able to call pendObjectForDestruction() when getting destroyed.
        friend class Object;
    
        #pragma region Game_State

        /// @brief Loads a room (a level). Clears all game data like game objects. Also creates base objects for the room.
        /// @return True on success, false otherwise.
        bool loadRoom();

        /// @brief Resets the game data including game objects and the camera.
        void resetGameData();

        #pragma endregion Game_State

        #pragma region Game_Objects

        /// @brief Destroys all of the game objects.
        void clearGameObjects();

        /// @brief Creates a basic object from the ObjectFactory, Adding the object to the ObjectManager.
        /// @tparam ...Args Extra arg types.
        /// @param objectName The name of the object to create.
        /// @param x X position.
        /// @param y Y position.
        /// @param ...args Extra args
        /// @return The object reference.
        template <typename... Args>
        Object* createBasicObject(std::string objectName, double x, double y, Args... args);

        /// @brief Creates a basic object from the ObjectFactory, Adding the object to the ObjectManager.
        /// @tparam ...Args Extra arg types.
        /// @param objectName The name of the object to create.
        /// @param x X position.
        /// @param y Y position.
        /// @param ...args Extra args
        /// @return The object reference.
        template <typename... Args>
        Object* createBasicObject(std::string objectName, double x, double y, std::vector<ParameterVariant> args);

        /// @brief Adds a game object to the object manager. This does NOT create the game object.
        ///        If the object is a Player, adds it to the Players list.
        ///        If the object is a Menu, adds it to the MenuManager.
        /// @param object The object to add. This should not be NULL.
        void addGameObject(Object *object);

        /// @brief Pends a game object for deletion by adding it to the destroy list. The game object will be marked as destroyed.
        /// @param object The game object to kill.
        void pendObjectForDestruction(Object *object);

        #pragma endregion Game_Objects

        #pragma region Input

        /// @brief Keyboard callback for debugging purposes.
        /// @param e Event info.
        void keyboardCallback(SDL_Event &e);

        #pragma endregion Input

        Engine *engine = NULL;
        ObjectManager *objectManager;
        Room *room = NULL;
        Camera *camera = NULL;

        bool gamePaused;

        bool readyToLoadRoom;
        std::string nextRoom;           //Either the room name or the rooms JSON string

        //Debugging
        bool showFPS;
};

#include "gameManager.tpp"

#endif
