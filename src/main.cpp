#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include <iostream>
#include "main.hpp"
#include "engine.hpp"
#include "config.hpp"
#include "dateUtils.hpp"
#include "fileUtils.hpp"

Engine *engine;

int main(int argc, char *argv[])
{
    SDL_SetAppMetadata(Config::GAME_TITLE, Config::GAME_VERSION, "com.ethan.engine");
    SDL_SetLogOutputFunction(&reportLog, NULL);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS)) 
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());

        return SDL_APP_FAILURE;
    }

    engine = new Engine();

    if (!engine->initializeEngine())
    {
        return SDL_APP_FAILURE;
    }

    engine->mainLoop();

    delete engine;
    engine = nullptr;

    SDL_Quit();

    return SDL_APP_SUCCESS;
}

void reportLog(void *userdata, int category, SDL_LogPriority priority, const char* message)
{
    std::cout << message << "\n";
    
    //Create the logs folder
    if (File::createFolder(L"logs/"))
    {
        std::string date;
        char filename[80] = "logs/";

        //Get log filename using date
        date = Date::getDateString();

        strcat(filename, date.c_str());
        strcat(filename, ".txt");

        //Save log to a text file
        SDL_IOStream* file = SDL_IOFromFile(filename, "a");

        if (file != NULL)
        {
            SDL_WriteIO(file, message, sizeof(char) * strlen(message));
            SDL_WriteIO(file, "\n", sizeof(char));

            SDL_CloseIO( file );
        }
    }

    //Display message box for errors
    if (priority == SDL_LOG_PRIORITY_ERROR && category == SDL_LOG_CATEGORY_ERROR)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error Occured", message, NULL);
    }
}