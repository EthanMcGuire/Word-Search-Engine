#pragma once

#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include <unordered_map>
#include <string>
#include <SDL3/SDL_render.h>

class SpriteManager
{
    public:
        SpriteManager();
        ~SpriteManager();

        bool loadTextures();

    private:
        std::unordered_map<std::string, SDL_Texture*> textures;
};

#endif