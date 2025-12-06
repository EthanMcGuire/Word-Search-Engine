#pragma once

#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL_log.h>

void cleanUp();
void setupLogging();
void reportLog(void *userdata, int category, SDL_LogPriority priority, const char* message);

#endif
