#pragma once

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <queue>
#include <string>

class Engine;
class Camera;
class Room;
class Random;
class AudioController;
class AssetManager;
class EventDispatcher;
class SDL_Renderer;

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

        /*
        /// @brief Checks if an error occured.
        /// @return True if an error occured, false otherwise.
        bool gameErrorOccured();

        /// @brief Gets a message title for an error that occured.
        /// @return The error title, or empty string if no error occured.
        std::string getGameErrorTitle();

        /// @brief Gets a message for an error that occured.
        /// @return The error that occured, or empty string if no error occured.
        std::string getGameErrorMessage();

        /// @brief Resets the game error message to empty string.
        void resetGameErrorMessage();
        */

        /// @brief Sets a room to load at the end of the frame.
        /// @param nextRoomToLoad The name of the room to load.
        void setRoomToLoad(std::string nextRoomToLoad);
        
        #pragma endregion Game_State

        #pragma region Collision

        #pragma endregion Collision

        #pragma region WrapperMethods

        /*
        /// @brief Pulls an animation from the animation manager.
        /// @param name The animation name.
        /// @return The animation info, or NULL.
        AnimationInfo* getAnimation(std::string name) const;

        /// @brief Pulls a bitmap font from the asset manager.
        /// @param name The font name.
        /// @return The font, or nullptr.
        BitmapFont* getBitmapFont(std::string name) const;
        */

        #pragma endregion WrapperMethods

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

        #pragma endregion Getters

    private:
    
        #pragma region Game_State

        /// @brief Loads a room (a level). Clears all game data like game objects. Also creates base objects for the room.
        /// @return True on success, false otherwise.
        bool loadRoom();

        /// @brief Resets the game data including game objects and the camera.
        void resetGameData();

        /// @brief Destroys all of the game objects.
        void clearGameObjects();

        #pragma endregion Game_State

        Engine *engine = NULL;
        Room *room = NULL;
        Camera *camera = NULL;

        bool gamePaused;

        bool readyToLoadRoom;
        std::string nextRoom;           //Either the room name or the rooms JSON string

        //Debugging
        bool showFPS;
};

#endif