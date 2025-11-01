#include "gameManager.hpp"
#include "audioController.hpp"
#include "assetManager.hpp"
#include "engine.hpp"
#include "room.hpp"
#include "cameraLerp.hpp"
#include "config.hpp"

GameManager::GameManager(Engine *engine)
{
    this->engine = engine;

    gamePaused = false;
    showFPS = true;

    room = new Room(engine->getAssetManager(), "DEFAULT", Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
    camera = new CameraLerp(this, gmtl::Vec2d(0, 0), Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
}

GameManager::~GameManager()
{
    delete room;
    delete camera;

    room = NULL;
    camera = NULL;
}

#pragma region Game_Loop

/// @brief Updates the game.
/// @param deltaTime Time change between last and current frame.
/// @return True on success, false on failure.
bool GameManager::update(double deltaTime)
{
    //Update background
    room->updateBackground(deltaTime);

    //Update objects
    //objectManager->updateGameObjects(deltaTime, isGamePaused);

    //Update camera
    if (!getGamePaused())
    {
        camera->update(room, deltaTime, getRandom());
    }

    //Load a new room
    if (readyToLoadRoom)
    {
        readyToLoadRoom = false;

        if (!loadRoom())
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: Failed to load room %s.", nextRoom);

            return false;
        }
    }

    return true;
}

/// @brief Draws the room background and the game objects.
/// @param renderer The game windows renderer.
void GameManager::draw(SDL_Renderer *renderer)
{
    //Draw background
    room->drawBackground(renderer, camera);

    //Draw objects
    //ObjectDrawer::drawGameObjects(objectManager, renderer, camera);
}

/// @brief Draws the game GUI. This includes the HUD and any debug information.
/// @param renderer The game windows renderer.
void GameManager::drawGui(SDL_Renderer *renderer)
{
    //Default font
    BitmapFont *font = getAssetManager()->getBitmap("fntOpenSans");

    if (font == NULL)
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Font fntOpenSans does not exist!");

        return;
    }

    //Draw game objects GUIs
    //ObjectDrawer::drawGameObjectGuis(objectManager, renderer);

    //Draw debug info
    if (showFPS)
    {
        std::string text = "FPS: ";
        text += std::to_string((int) floor(engine->getFPS()));

        font->drawText(renderer, 0, 0, text);
    }
}

#pragma endregion Game_Loop

/// @brief Ends the game.
void GameManager::endGame() const
{
    engine->endGame();
}

/// @brief Pauses the game.
void GameManager::pauseGame()
{
    gamePaused = true;
}

/// @brief Unpauses the game.
void GameManager::unpauseGame()
{
    gamePaused = false;
}

/// @brief Sets a room to load at the end of the frame.
/// @param nextRoomToLoad The name of the room to load.
void GameManager::setRoomToLoad(std::string nextRoomToLoad)
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Set next room to load: %s", nextRoomToLoad.c_str());

    readyToLoadRoom = true;
    nextRoom = nextRoomToLoad;
}

/// @brief Loads a room (a level). Clears all game data like game objects. Also creates base objects for the room.
/// @return True on success, false otherwise.
bool GameManager::loadRoom()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Loading room %s", nextRoom.c_str());

    resetGameData();

    if (!room->loadRoomByName(nextRoom))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: Failed to load room.");

        return false;
    }

    //Set the camera position
    camera->setPosition(room->getCameraStartX(), room->getCameraStartY());

    //Create game objects
    std::vector<ObjectInfo> objects = room->getRoomObjects();

    for (const auto& object : objects)
    {
        //createObject(object.objectName, object.x, object.y, object.parameters);
    }

    //Play music
    AudioController *audioController;
    std::string musicName;

    audioController = engine->getAudioController();

    musicName = room->getRoomMusic();

    if (musicName != "")
    {
        if (!audioController->playMusic(musicName))
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "GameManager: Failed to play music for the room: %s", nextRoom);

            return false;
        }
    }
    else
    {
        audioController->stopMusic();
    }

    return true;
}

/// @brief Resets the game data including game objects and the camera.
void GameManager::resetGameData()
{
    clearGameObjects();

    //Reset the room
    room->clearRoom();

    //Reset the camera
    camera->setPosition(0, 0);
}

/// @brief Destroys all of the game objects.
void GameManager::clearGameObjects()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "GameManager: Clearing all game objects...");

    /*
    menuManager->clearMenus();          //This needs to be done before freeGameObjects, as calling freeGameObjects first will result in null destroy calls
    objectManager->freeGameObjects();
    
    players.clear();
    
    for (int i = 0; i < maxLocalPlayers; i++)
        localPlayers[i] = nullptr;
    */
}

#pragma region Getters

/// @return True if the game is paused, false otherwise.
bool GameManager::getGamePaused()
{
    return gamePaused;
}

/// @return The game camera.
Camera* GameManager::getCamera() const
{
    return camera;
}

/// @return The current room.
Room* GameManager::getRoom() const
{
    return room;
}

/// @return The Random number generator class.
Random* GameManager::getRandom() const
{
    return engine->getRandomNumberGenerator();
}

/// @return The asset manager. Used to pull various assets.
AssetManager* GameManager::getAssetManager() const
{
    return engine->getAssetManager();
}

/// @return The audio controller. Used to play music and sounds.
AudioController* GameManager::getAudioController() const
{
    return engine->getAudioController();
}

/// @return The EventDispatcher. Used to add event listeners and dispatch events.
EventDispatcher* GameManager::getEventDispatcher() const
{
    return engine->getEventDispatcher();
}

#pragma endregion Getters