#include "gameManager.hpp"
#include "audioController.hpp"
#include "engine.hpp"
#include "room.hpp"
#include "cameraLerp.hpp"
#include "config.hpp"

GameManager::GameManager(Engine *engine)
{
    this->engine = engine;

    gamePaused = false;
    showFPS = false;

    room = new Room(engine->getAssetManager(), "DEFAULT", Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
    camera = new CameraLerp(this, gmtl::Vec2d(0, 0), Config::SCREEN_WIDTH, Config::SCREEN_HEIGHT);
}

GameManager::~GameManager()
{
    
}

/// @brief Updates the game.
/// @param deltaTime Time change between last and current frame.
/// @return True on success, false on failure.
bool GameManager::update(double deltaTime)
{
    return true;
}

void GameManager::startGame()
{
    AudioController *audioController;

    //Play music
    audioController = engine->getAudioController();

    audioController->playMusic(MUSIC_SOURCE_MUSIC_1);
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