#pragma once

#ifndef COLLISION_DRAWER_H
#define COLLISION_DRAWER_H

#include "collision.hpp"
#include "SDL3/SDL_render.h"

class Camera;

/// @brief Stores various methods of collision drawing.
namespace CollisionDrawer
{
    /// @brief Draws a collision debug.
    /// @param renderer The renderer to draw to.
    /// @param camera The game camera. Set to null if no camera is being used (GUI).
    /// @param bBox Collision to draw.
    /// @param color The color of the collision.
    void drawCollision(SDL_Renderer* renderer, Camera *camera, BBox bBox, SDL_Color color = {255, 0, 0, 128});
}

#endif