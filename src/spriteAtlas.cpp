#include "spriteAtlas.hpp"
#include <SDL3/SDL_log.h>

SpriteAtlas::SpriteAtlas()
{
    clearAtlasInfo();
}

SpriteAtlas::~SpriteAtlas()
{
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SpriteAtlas: Atlas freed");

    freeTexture();
}

/// @brief Clears the sprite atlas variables.
void SpriteAtlas::clearAtlasInfo()
{
    spriteCount = 0;
    columnCount = 0;
    rowCount = 0;
    spriteWidth = 0;
    spriteHeight = 0;   
}

/// @brief Loads the given image file as a sprite sheet.
/// @param renderer The renderer the texture will be created for.
/// @param path Image file path.
/// @param spriteFrameWidth The width of individual sprites on the sprite sheet.
/// @param spriteFrameHeight The height of individual sprites on the sprite sheet.
/// @return True on success.
bool SpriteAtlas::loadTexture(SDL_Renderer *renderer, std::string path, int spriteFrameWidth, int spriteFrameHeight)
{
    clearAtlasInfo();

    //loadTexture will clear the current texture
    if (!Texture::loadTexture(renderer, path))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SpriteAtlas: Failed to load texture!");

        return false;
    }

    //Get sprite atlas info
    spriteWidth = spriteFrameWidth;
    spriteHeight = spriteFrameHeight;

    columnCount = getWidth() / spriteWidth;
    rowCount = getHeight() / spriteHeight;
    spriteCount = columnCount * rowCount;

    //No sprites in the sheet?
    if (spriteCount == 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SpriteAtlas: Failed to apply Sprite Atlas. Sprite Frame size is too large to fit in the texture.");

        clearAtlasInfo();
        freeTexture();

        return false;
    }

    return true;
}

/// @brief Renders the given sprite of a sprite sheet to the given renderer.
/// @param renderer The renderer to render to.
/// @param spriteIndex The sprite to draw from the sprite sheet.
/// @param x X location on the viewport.
/// @param y Y location on the viewport.
/// @param xScale Width scale of texture destination.
/// @param yScale Height scale of texture destination.
/// @param clip Portion of the texture to render.
/// @param angle Image rotation angle.
/// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
/// @param flip Whether the flip the texture horizontally or vertically.
void SpriteAtlas::render(SDL_Renderer *renderer, unsigned int spriteIndex, int x, int y, double xScale, double yScale, SDL_FRect *clip, double angle, SDL_FPoint *center, SDL_FlipMode flip) const
{
    if (!textureLoaded())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SpriteAtlas: Attempted to render sprite with no load sprite atlas! Call loadTexture() first.");

        return;
    }
    else if (spriteIndex < 0 || spriteIndex >= spriteCount)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SpriteAtlas: Failed to render sprite. spriteIndex is outside the valid range. spriteIndex: %d ValidRange: 0 -> %d", spriteIndex, spriteCount - 1);

        return;
    }

    SDL_FRect spriteClip;
    int col, row;

    col = spriteIndex % columnCount;
    row = spriteIndex / columnCount;

    //Get the sprites region in the sprite atlas
    spriteClip.x = (float) col * spriteWidth;
    spriteClip.y = (float) row * spriteHeight;
    spriteClip.w = (float) spriteWidth;
    spriteClip.h = (float) spriteHeight;

    //Render part of the sprite
    if (clip != NULL)
    {
        spriteClip.x += clip->x;
        spriteClip.y += clip->y;
        spriteClip.w = clip->w;
        spriteClip.h = clip->h;
    }

    //Render the sprite
    Texture::render(renderer, x, y, xScale, yScale, &spriteClip, angle, center, flip);
}

/// @brief Renders the given sprite of a sprite sheet to the given renderer.
/// @param renderer The renderer to render to.
/// @param spriteIndex The sprite to draw from the sprite sheet.
/// @param x X location on the viewport.
/// @param y Y location on the viewport.
/// @param drawWidth Width to draw the texture at.
/// @param drawHeight Height to draw the texture at.
/// @param clip Portion of the texture to render.
/// @param angle Image rotation angle.
/// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
/// @param flip Whether the flip the texture horizontally or vertically.
void SpriteAtlas::renderStretched(SDL_Renderer *renderer, unsigned int spriteIndex, int x, int y, int drawWidth, int drawHeight, SDL_FRect *clip, double angle, SDL_FPoint *center, SDL_FlipMode flip) const
{
    if (!textureLoaded())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SpriteAtlas: Attempted to render sprite with no load sprite atlas! Call loadTexture() first.");

        return;
    }
    else if (spriteIndex < 0 || spriteIndex >= spriteCount)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SpriteAtlas: Failed to render sprite. spriteIndex is outside the valid range. spriteIndex: %d ValidRange: 0 -> %d", spriteIndex, spriteCount - 1);

        return;
    }

    SDL_FRect spriteClip;
    int col, row;

    col = spriteIndex % columnCount;
    row = spriteIndex / columnCount;

    //Get the sprites region in the sprite atlas
    spriteClip.x = (float) col * spriteWidth;
    spriteClip.y = (float) row * spriteHeight;
    spriteClip.w = (float) spriteWidth;
    spriteClip.h = (float) spriteHeight;

    //Render part of the sprite
    if (clip != NULL)
    {
        spriteClip.x += clip->x;
        spriteClip.y += clip->y;
        spriteClip.w = clip->w;
        spriteClip.h = clip->h;
    }

    //Render the sprite
    Texture::renderStretched(renderer, x, y, drawWidth, drawHeight, &spriteClip, angle, center, flip);
}

/// @brief Renders the given sprite of a sprite sheet to the given renderer. Repeates the sprite over a area.
/// @param renderer The renderer to render to.
/// @param spriteIndex The sprite to draw from the sprite sheet.
/// @param x X location on the viewport.
/// @param y Y location on the viewport.
/// @param drawWidth Width to draw the texture at.
/// @param drawHeight Height to draw the texture at.
/// @param clip Portion of the texture to render.
/// @param angle Image rotation angle.
/// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
/// @param flip Whether the flip the texture horizontally or vertically.
void SpriteAtlas::renderRepeated(SDL_Renderer *renderer, unsigned int spriteIndex, int x, int y, int drawWidth, int drawHeight, SDL_FRect *clip, double angle, SDL_FPoint *center, SDL_FlipMode flip) const
{
    if (!textureLoaded())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SpriteAtlas: Attempted to render sprite with no load sprite atlas! Call loadTexture() first.");

        return;
    }
    else if (spriteIndex < 0 || spriteIndex >= spriteCount)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SpriteAtlas: Failed to render sprite. spriteIndex is outside the valid range. spriteIndex: %d ValidRange: 0 -> %d", spriteIndex, spriteCount - 1);

        return;
    }

    SDL_FRect spriteClip;
    int col, row;

    col = spriteIndex % columnCount;
    row = spriteIndex / columnCount;

    //Get the sprites region in the sprite atlas
    spriteClip.x = (float) col * spriteWidth;
    spriteClip.y = (float) row * spriteHeight;
    spriteClip.w = (float) spriteWidth;
    spriteClip.h = (float) spriteHeight;

    //Render part of the sprite
    if (clip != NULL)
    {
        spriteClip.x += clip->x;
        spriteClip.y += clip->y;
        spriteClip.w = clip->w;
        spriteClip.h = clip->h;
    }

    //Render the sprite
    Texture::renderRepeated(renderer, x, y, drawWidth, drawHeight, &spriteClip, angle, center, flip);
}

#pragma region Getters

unsigned int SpriteAtlas::getSpriteCount() const
{
    return spriteCount;
}

unsigned int SpriteAtlas::getColumnCount() const
{
    return columnCount;
}

unsigned int SpriteAtlas::getRowCount() const
{
    return rowCount;
}

int SpriteAtlas::getSpriteWidth() const
{
    return spriteWidth;
}

int SpriteAtlas::getSpriteHeight() const
{
    return spriteHeight;
}

#pragma endregion Getters
