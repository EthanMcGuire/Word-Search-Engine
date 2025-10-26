#include "gameController.hpp"
#include "audioController.hpp"
#include "engine.hpp"

GameController::GameController(Engine *engine)
{
    this->engine = engine;
}

GameController::~GameController()
{
    
}

/// @brief Updates the game.
/// @param deltaTime Time change between last and current frame.
/// @return True on success, false on failure.
bool GameController::update(double deltaTime)
{
    return true;
}

void GameController::startGame()
{
    AudioController *audioController;

    //Play music
    audioController = engine->getAudioController();

    audioController->playMusic(MUSIC_SOURCE_MUSIC_1);
}