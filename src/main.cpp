#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <string>
#include <iostream>
#include <filesystem>
#include "main.hpp"
#include "engine.hpp"
#include "config.hpp"
#include "dateUtils.hpp"
#include "fileUtils.hpp"

Engine *engine;

int main(int argc, char *argv[])
{
    SDL_SetAppMetadata(Config::GAME_TITLE, Config::GAME_VERSION, "com.ethan.engine");
    
    SDL_Log("Starting game. Name: %s. Game version: %s", Config::GAME_TITLE, Config::GAME_VERSION);

    setupLogging();

    engine = new Engine();

    if (!engine->initializeEngine())
    {
        cleanUp();

        return 1;
    }

    engine->startGame();

    SDL_Log("Starting the main loop.");

    engine->mainLoop();

    cleanUp();

    return 0;
}

void cleanUp()
{
    SDL_Log("Cleaning up...");

    engine->closeEngine();

    delete engine;
    engine = NULL;

    SDL_Log("Finished cleaning up");
}

void setupLogging()
{
	//Create the logs folder
	if (!File::createFolder(L"logs/"))
    	{
		SDL_Log("Failed to create the logs folder. Logging will proceed without writing to a file.");	
	}

	SDL_SetLogOutputFunction(&reportLog, NULL);
}

void reportLog(void *userdata, int category, SDL_LogPriority priority, const char* message)
{
	std::cout << message << "\n";
    
	if (std::filesystem::exists("logs/"))
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
