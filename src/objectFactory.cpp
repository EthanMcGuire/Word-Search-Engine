#include "objectFactory.hpp"
#include "gameManager.hpp"
#include "object.hpp"
#include "stringUtils.hpp"
//#include "player.hpp"
//#include "solid.hpp"
#include "config.hpp"
#include <SDL3/SDL_log.h>
#include "objGameClock.hpp"

//Client headers
/*
#ifdef CLIENT
    #include "messageBoxContainer.hpp"
    #include "messageBoxMenu.hpp"
    #include "button.hpp"
    #include "gridContainer.hpp"
    #include "buttonContainer.hpp"
    #include "label.hpp"
    //#include "menuSlider.hpp"
    #include "scrollbar.hpp"
    #include "titleScreen.hpp"
    #include "loadingScreen.hpp"
#endif
*/

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
    /*
    //Client objects
    #ifdef CLIENT

    if (objectName == "loadingscreen")
    {
        return new LoadingScreen(gameManager, x, y);
    }
    //TODO
    //ADD BACK
    else if (objectName == "titlescreen")
    {
        return new TitleScreen(gameManager, x, y);
    }
    else if (objectName == "label")
    {
         return new Label(gameManager, x, y);
    }
    else if (objectName == "messageboxcontainer")
    {
        return new MessageBoxContainer(gameManager, x, y);
    }
    else if (objectName == "messageboxmenu")
    {
        return new MessageBoxMenu(gameManager, x, y);
    }
    else if (objectName == "gridcontainer")
    {
        return new GridContainer(gameManager, x, y);
    } 
    else if (objectName == "buttoncontainer")
    {
        return new ButtonContainer(gameManager, x, y);
    } 
    else if (objectName == "button")
    {
        return new Button(gameManager, x, y);
    }
    else if (objectName == "scrollbar")
    {
        return new Scrollbar(gameManager, x, y);
    }
    else 
    
    #endif
    
    //Global objects
    if (objectName == "player")
    {
        return new Player(gameManager, x, y);
    }
    else if (objectName == "solid")
    {
        if (args.size() < 2)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "ObjectFactory: Solid object requires at least 2 additional arguments. Failed to create.");

            return nullptr;
        }

        try
        {
            return new Solid(gameManager, x, y, std::get<int>(args[0]), std::get<int>(args[1]));
        }
        catch (const std::bad_variant_access&)
        {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "ObjectFactory: Failed to create Solid. Invalid parameter types given! Expected format: %%f, %%f, %%i, %%i");

            return nullptr;
        }
    }
    */

    //Object doesn't exist
    SDL_LogInfo(SDL_LOG_CATEGORY_ERROR, "ObjectFactory: Object type %s does not exist. Failed to create.", objectName.c_str());

    return nullptr;
}
