#include "draw.hpp"
#include "spriteAtlas.hpp"
#include "config.hpp"
#include "camera.hpp"
#include "collision.hpp"
#include <SDL3/SDL_log.h>

/// @brief Used to draw sprites for game Objects. Draws a sprite from a sprite atlas with position, origin, scale, angle, alpha, and colorMod.
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
void Draw::drawSpriteExt(SDL_Renderer *renderer, Camera *camera, SpriteAtlas *atlas, int spriteIndex, gmtl::Vec2d pos, gmtl::Vec2i origin, gmtl::Vec2d imageScale, double imageAngle, 
                    int alpha, gmtl::Vec3i colorMod)
{
    if (renderer == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "drawSpriteExt(): Renderer is NULL! Failed to draw.");

        return;
    }

    if (atlas == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "drawSpriteExt(): Atlas is NULL! Failed to draw.");

        return;
    }

    gmtl::Vec2d drawPos;
    gmtl::Vec2d drawOrigin;
    gmtl::Vec2d drawScale;
    SDL_Point center;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    int drawWidth, drawHeight;
    double cameraX, cameraY;
    
    //Scale cannot be negative (Instead we will change the RendererFlip)
    drawScale = gmtl::Vec2d(abs(imageScale[0]), abs(imageScale[1]));

    drawWidth = atlas->getSpriteWidth() * drawScale[0];
    drawHeight = atlas->getSpriteHeight() * drawScale[1];

    //Get draw origin
    drawOrigin = gmtl::Vec2d(drawScale[0] * origin[0], drawScale[1] * origin[1]);

    center = {(int) drawOrigin[0], (int) drawOrigin[1]};

    //Get draw location
    drawPos = pos - drawOrigin;

    //Get the camera info
    cameraX = camera->getDrawX();
    cameraY = camera->getDrawY();

    //Only draw if we are drawing in the camera
    CollisionRect rect = {drawPos[0], drawPos[1], drawWidth, drawHeight};

    if (Collision::checkCollisionRects(rect, camera->getDrawRect()))
    {
        //Account for negative scale
        if (imageScale[0] < 0 && imageScale[1] < 0)
        {
            flip = (SDL_RendererFlip) (SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
        }
        else if (imageScale[0] < 0)
        {
            flip = SDL_FLIP_HORIZONTAL;
        }
        else if (imageScale[1] < 0)
        {
            flip = SDL_FLIP_VERTICAL;
        }

        //Apply texture color and alpha
        atlas->setColorMod(colorMod[0], colorMod[1], colorMod[2]);
        atlas->setAlpha(alpha);

        //Draw the current sprite, offsetting by the cameras position
        atlas->render(renderer, spriteIndex, floor(drawPos[0] - cameraX), floor(drawPos[1] - cameraY), drawScale[0], drawScale[1], NULL, imageAngle, &center, flip);

        //Reset texture color
        atlas->setColorMod(255, 255, 255);
        atlas->setAlpha(255);
    }
    else
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "drawSpriteExt(): DIDNT DRAW. OUTSIDE OF CAMERA");
    }
}