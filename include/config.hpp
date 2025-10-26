#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

namespace Config
{
    static const char* GAME_TITLE = "Test Game";
    static const char* GAME_VERSION = "1.0";

    static const int SCREEN_WIDTH = 640;
    static const int SCREEN_HEIGHT = 360;

    static const int FPS = 60;
    static const int TICKS_PER_FRAME = 1000 / FPS;
};

#endif