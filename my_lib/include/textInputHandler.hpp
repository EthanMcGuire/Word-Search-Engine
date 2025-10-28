#pragma once

#ifndef TEXT_INPUT_HANDLER_H
#define TEXT_INPUT_HANDLER_H

#include <SDL_events.h>
#include <string>
#include <queue>

class GameManager;

class TextInputHandler
{
    public:
        TextInputHandler(GameManager *gameManager, std::string defaultText = "", int maxCharacters = 20);
        ~TextInputHandler();

        /// @brief Tells SDL to start getting text input, and allows the callbacks to read inputs.
        void startGettingTextInput();

        /// @brief Tells SDL to stop getting text input, and disallows the callbacks to read inputs.
        void stopGettingTextInput();

        /// @return The current input text.
        std::string getText();

    private:
        /// @brief Handles the special keyboard events backspace, copy, and paste.
        /// @param e The SDL event.
        void keyboardCallback(SDL_Event &e);

        /// @brief Adds text to the input.
        /// @param e The SDL event.
        void textInputCallback(SDL_Event &e);

        GameManager *gameManager;
        
        std::string inputText;
        bool gettingTextInput;
        int maxCharacters;

        std::queue<uint32_t> eventListenerIds;  //Used to remove listeners from the EventDispatcher
};

#endif