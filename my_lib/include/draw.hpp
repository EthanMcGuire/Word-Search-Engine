#pragma once

#ifndef DRAW_H
#define DRAW_H

#include <SDL_render.h>
#include <gmtl/gmtl.h>
#include <gmtl/Matrix.h>

class SpriteAtlas;
class Camera;

namespace Draw
{
    /// @brief Draws a sprite from a sprite atlas with position, origin, scale, angle, alpha, and colorMod.
    /// @param renderer The video renderer to draw to.
    /// @param camera The game camera that determines when to draw.
    /// @param atlas The sprite atlas to draw from.
    /// @param spriteIndex The sprite index to draw.
    /// @param pos X, Y location.
    /// @param origin X, Y origin.
    /// @param imageScale X and Y scale.
    /// @param imageAngle Rotation angle.
    /// @param alpha Image alpha. 0 for completely transparent. 255 for fully opaque.
    /// @param colorMod The RGB color mod.
    void drawSpriteExt(SDL_Renderer *renderer, Camera *camera, SpriteAtlas *atlas, int spriteIndex, gmtl::Vec2d pos, gmtl::Vec2i origin, gmtl::Vec2d imageScale, double imageAngle, 
                        int alpha, gmtl::Vec3i colorMod);
}

#endif