#include "textInputHandler.hpp"
#include "gameManager.hpp"
#include "eventDispatcher.hpp"
#include <SDL_stdinc.h>
#include <cassert>
#include <SDL_clipboard.h>

TextInputHandler::TextInputHandler(GameManager *gameManager, std::string defaultText, int maxCharacters)
{
    assert(gameManager != nullptr);

    this->gameManager = gameManager;
    this->inputText = defaultText;
    this->maxCharacters = SDL_max(maxCharacters, (int) inputText.length());

    gettingTextInput = false;

    //Set up input events
    EventDispatcher *dispatcher = gameManager->getEventDispatcher();

    eventListenerIds.push(dispatcher->addSDLListener(SDL_KEYDOWN, [this](SDL_Event e) {
        this->keyboardCallback(e);
    }));
    eventListenerIds.push(dispatcher->addSDLListener(SDL_KEYUP, [this](SDL_Event e) {
        this->keyboardCallback(e);
    }));
    eventListenerIds.push(dispatcher->addSDLListener(SDL_TEXTINPUT, [this](SDL_Event e) {
        this->textInputCallback(e);
    }));
}

TextInputHandler::~TextInputHandler()
{
    //Clear listeners
    gameManager->getEventDispatcher()->removeSDLListenersByIds(eventListenerIds);
}

/// @brief Tells SDL to start getting text input, and allows the callbacks to read inputs.
void TextInputHandler::startGettingTextInput()
{
    gettingTextInput = true;

    SDL_StartTextInput();
}

/// @brief Tells SDL to stop getting text input, and disallows the callbacks to read inputs.
void TextInputHandler::stopGettingTextInput()
{
    gettingTextInput = false;

    SDL_StopTextInput();
}

/// @return The current input text.
std::string TextInputHandler::getText()
{
    return inputText;
}

#pragma region InputCallbacks

/// @brief Handles the special keyboard events backspace, copy, and paste.
/// @param e The SDL event.
void TextInputHandler::keyboardCallback(SDL_Event &e)
{
    if (!gettingTextInput)
    {
        return;
    }

    //Do nothing on release
    if (e.key.state == SDL_RELEASED)
    {
        return;
    }

    //Handle special key input

    //Backspace
    if (e.key.keysym.sym == SDLK_BACKSPACE && inputText.length() > 0)
    {
        inputText.pop_back();
    }

    //Copy
    else if (e.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL)
    {
        SDL_SetClipboardText(inputText.c_str());
    }

    //Paste
    else if (e.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL)
    {
        char* pasteText = SDL_GetClipboardText();

        inputText += pasteText;

        //Free since GetClipboardText() returns a newly allocated string
        SDL_free(pasteText);

        //Limit the number of characters
        if (((int) inputText.length()) > maxCharacters)
        {
            inputText = inputText.substr(0, maxCharacters);
        }
    }
}

/// @brief Adds text to the input.
/// @param e The SDL event.
void TextInputHandler::textInputCallback(SDL_Event &e)
{
    if (!gettingTextInput)
    {
        return;
    }

    //Ignore if this is a copy or paste
    if (SDL_GetModState() & KMOD_CTRL)
    {
        if (e.text.text[0] == 'c' || e.text.text[0] == 'C')
        {
            return;
        }

        if (e.text.text[0] == 'v' || e.text.text[0] == 'V')
        {
            return;
        }
    }

    //Append characters
    inputText += e.text.text;

    //Limit the number of characters
    if (((int) inputText.length()) > maxCharacters)
    {
        inputText = inputText.substr(0, maxCharacters);
    }
}

#pragma endregion InputCallbacks