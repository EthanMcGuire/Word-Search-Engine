#pragma once

#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <queue>

class Engine;
class Camera;
class Room;
class Random;
class AssetManager;
class EventDispatcher;

class GameManager
{
    public:

        GameManager(Engine *engine);
        ~GameManager();
        
        #pragma region Game_Loop

        /// @brief Updates the game.
        /// @param deltaTime Time change between last and current frame.
        /// @return True on success, false on failure.
        bool update(double deltaTime);

        /*
        /// @brief Draws the room background and the game objects.
        /// @param renderer The game windows renderer.
        void draw(SDL_Renderer *renderer);

        /// @brief Draws the game GUI. This includes the HUD and any debug information.
        /// @param renderer The game windows renderer.
        void draw(SDL_Renderer *renderer);
        */

        #pragma endregion Game_Loop

        #pragma region GameControl

        /// @brief Starts the game.
        void startGame();

        /*
        /// @brief Ends the game.
        void endGame() const;

        /// @brief Pauses the game by disabling the players input. If this isn't a online game, update events will be ignored until the game is unpaused.
        void pauseGame();

        /// @brief Unpauses the game, enabling the players input. If this isn't a online game, update events will be resumed.
        void unpauseGame();
        */

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

        /// @brief Sets a room to load at the end of the frame.
        /// @param nextRoomToLoad The room to load. Either the name of the room or a JSON object describing the room.
        /// @param isJson Whether the room to load is a JSON.
        void setRoomToLoad(std::string nextRoomToLoad, bool isJson);
        */

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

        #pragma region Game_Loop

        /*
        /// @brief Updates the games room, camera, and game objects.
        /// @param deltaTime Time change in seconds since last frame.
        /// @param isGamePaused Certain objects and the camera will not be updated if this is true.
        void updateGameData(double deltaTime, bool isGamePaused);
        */

        #pragma endregion Game_Loop
    
        #pragma region Game_State

        /*
        /// @brief Sets the game state. Loading a new Room based on the state.
        /// @param newState The new state
        void setState(GAME_STATE newState);
        
        /// @brief Loads a room (a level). Clears all game data like game objects. Also creates base objects for the room.
        /// @return True on success, false otherwise.
        bool loadRoom();

        /// @brief Resets the game data including game objects, tilemaps, and the camera.
        void resetGameData();

        /// @brief Destroys all of the game objects, and resets the clients player server ID.
        void clearGameObjects();
        */

        #pragma endregion Game_State

        Engine *engine;
        Room *room;
        Camera *camera;

        std::queue<uint32_t> eventListenerIds;  //Used to remove listeners from the EventDispatcher

        //State control
        //GAME_STATE gameState;
        bool gamePaused;

        bool readyToLoadRoom;
        bool nextRoomIsJson;            //Whether the next room to load is a JSON, or a room name
        std::string nextRoom;           //Either the room name or the rooms JSON string

        //Debugging
        bool showFPS;
};

#endif