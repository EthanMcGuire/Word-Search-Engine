#include "gameManager.hpp"
#include "audioController.hpp"
#include "engine.hpp"

GameManager::GameManager(Engine *engine)
{
    this->engine = engine;
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