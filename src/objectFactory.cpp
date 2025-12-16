#include "objectFactory.hpp"
#include "gameManager.hpp"
#include "object.hpp"
#include "stringUtils.hpp"
#include "config.hpp"
#include <SDL3/SDL_log.h>
#include "objGameClock.hpp"
#include "objTitleScreen.hpp"
#include "objWordSearchController.hpp"
#include "objWordSearchBox.hpp"
#include "objRose.hpp"

Object* ObjectFactory::createObject(GameManager *gameManager, std::string objectName, double x, double y, std::vector<ParameterVariant> args)
{
    if (gameManager == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "ObjectFactory: gameManager is NULL. Objects need a gameManger to be created.");

        return nullptr;
    }

    //Create object by name
    objectName = stringToLower(objectName);

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "ObjectFactory: Creating game object of type %s...", objectName.c_str());

    if (objectName == "objgameclock")
    {
	    return new ObjGameClock(gameManager, x, y);
    }
    else if (objectName == "objtitlescreen")
    {
	    return new ObjTitleScreen(gameManager, x, y);
    }
    else if (objectName == "objwordsearchcontroller")
    {
	    return new ObjWordSearchController(gameManager, x, y);
    }
    else if (objectName == "objwordsearchbox")
    {
	    return new ObjWordSearchBox(gameManager, x, y);
    }
    else if (objectName == "objrose")
    {
	    return new ObjRose(gameManager, x, y, std::get<double>(args[0]), std::get<double>(args[1]), std::get<int>(args[2]));
    }
    //
    //Object doesn't exist
    SDL_LogInfo(SDL_LOG_CATEGORY_ERROR, "ObjectFactory: Object type %s does not exist. Failed to create.", objectName.c_str());

    return nullptr;
}
