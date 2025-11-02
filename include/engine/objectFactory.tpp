#include "objectFactory.hpp"
#include "object.hpp"
#include <SDL3/SDL_log.h>

template <typename... Args>
Object* ObjectFactory::createObject(GameManager *gameManager, std::string objectName, double x, double y, Args... args)
{
    std::vector<ParameterVariant> vArgs = { ParameterVariant(args)... };

    return createObject(gameManager, objectName, x, y, vArgs);
}