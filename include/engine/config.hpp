#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include <SDL3/SDL.h>
#include <string>

namespace Config
{
    static const char* GAME_TITLE = "Test Game";
    static const char* GAME_VERSION = "1.0";
    static const char* STARTING_ROOM = "default";

    static const int SCREEN_WIDTH = 960;
    static const int SCREEN_HEIGHT = 540;

    static const int FPS = 60;
    static const int TICKS_PER_FRAME = 1000 / FPS;

#ifdef __linux__
    static const std::string ROOM_PATH = "assets/rooms"; 
    static const std::string ASSET_PATH = "assets/data/assets.json"; 
    //static const std::string ANIMATION_PATH = "resources/data/animations.json";
#else
    static const std::string ROOM_PATH = "assets\\rooms"; 
    static const std::string ASSET_PATH = "assets\\data\\assets.json"; 
    //static const std::string ANIMATION_PATH = "resources\\data\\animations.json";
#endif

    static const Uint32 SDL_FLAGS = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS;
    static const int WINDOW_FLAGS = SDL_WINDOW_RESIZABLE;
};

#endif
