#include "nineSlice.hpp"
#include "spriteAtlas.hpp"
#include <algorithm>

NineSlice::NineSlice()
{
    spriteSize = 0;
    minSize = 0;
    width = 0;
    height = 0;
}

/// @brief Sets the nine slice sprite atlas and sprite indices.
/// @param atlas The sprite atlas to use.
void NineSlice::setSpriteInfo(NineSliceSpriteInfo spriteInfo)
{
    clearSpriteInfo();

    //Do nothing if the atlas is null
    if (spriteInfo.atlas == NULL)
    {
        return;
    }
    
    this->spriteInfo = spriteInfo;

    spriteSize = this->spriteInfo.atlas->getSpriteWidth();
    minSize = spriteSize * 2;
}

/// @brief Clears the sprite info.
void NineSlice::clearSpriteInfo()
{
    this->spriteInfo.atlas = NULL;
}

/// @brief Draws the nine slice rectangle.
/// @param renderer The renderer.
/// @param x X draw location.
/// @param y Y draw location.
/// @param clip Portion of the texture to render for each slice.
/// @param angle Angle to rotate each slice.
/// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
/// @param flip Whether to flip each slice horizontally or vertically.
/// @param alpha Image alpha. 0 for completely transparent. 255 for fully opaque.
/// @param colorMod The RGB color mod.
void NineSlice::render(SDL_Renderer *renderer, int x, int y, SDL_FRect *clip, double angle, SDL_FPoint *center, SDL_FlipMode flip, int alpha, gmtl::Vec3i colorMod) const
{
    if (spriteInfo.atlas == NULL || width == 0 || height == 0 || spriteSize == 0)
    {
        return;
    }

    unsigned int drawWidth, drawHeight;

    //Limit width/height to minSize
    drawWidth = std::max(minSize, width);
    drawHeight = std::max(minSize, height);

    //Get the width and height for each section
    //The corners will be scaled at spriteSize
    unsigned int sliceWidth[9];
    unsigned int sliceHeight[9];
    unsigned int stretchedWidth;
    unsigned int stretchedHeight;

    stretchedWidth = drawWidth - spriteSize * 2;
    stretchedHeight = drawHeight - spriteSize * 2;

    sliceWidth[0] = spriteSize;                 //Top left corner
    sliceWidth[1] = stretchedWidth;
    sliceWidth[2] = spriteSize;                 //Top right corner
    sliceWidth[3] = spriteSize;
    sliceWidth[4] = stretchedWidth;
    sliceWidth[5] = spriteSize;
    sliceWidth[6] = spriteSize;                 //Bottom left corner
    sliceWidth[7] = stretchedWidth;
    sliceWidth[8] = spriteSize;                 //Bottom right corner

    sliceHeight[0] = spriteSize;                 //Top left corner
    sliceHeight[1] = spriteSize;
    sliceHeight[2] = spriteSize;                 //Top right corner
    sliceHeight[3] = stretchedHeight;
    sliceHeight[4] = stretchedHeight;
    sliceHeight[5] = stretchedHeight;
    sliceHeight[6] = spriteSize;                 //Bottom left corner
    sliceHeight[7] = spriteSize;
    sliceHeight[8] = spriteSize;                 //Bottom right corner

    //Apply texture color and alpha
    spriteInfo.atlas->setColorMod(colorMod[0], colorMod[1], colorMod[2]);
    spriteInfo.atlas->setAlpha(alpha);
    spriteInfo.atlas->setScaleMode(SDL_SCALEMODE_NEAREST);

    //Draw the nine-slice
    drawSlice(renderer, 0, spriteInfo.stretchLeft, x, y, sliceWidth[0], sliceHeight[0], clip, angle, center, flip);
    drawSlice(renderer, 1, spriteInfo.stretchTop, x + sliceWidth[0], y, sliceWidth[1], sliceHeight[1], clip, angle, center, flip);
    drawSlice(renderer, 2, spriteInfo.stretchRight, x + sliceWidth[0] + sliceWidth[1], y, sliceWidth[2], sliceHeight[2], clip, angle, center, flip);
    drawSlice(renderer, 3, spriteInfo.stretchMiddleLeft, x, y + sliceHeight[0], sliceWidth[3], sliceHeight[3], clip, angle, center, flip);
    drawSlice(renderer, 4, spriteInfo.stretchMiddleCenter, x + sliceWidth[0], y + sliceHeight[0], sliceWidth[4], sliceHeight[4], clip, angle, center, flip);
    drawSlice(renderer, 5, spriteInfo.stretchMiddleRight, x + sliceWidth[0] + sliceWidth[1], y + sliceHeight[0], sliceWidth[5], sliceHeight[5], clip, angle, center, flip);
    drawSlice(renderer, 6, spriteInfo.stretchBottomLeft, x, y + sliceHeight[0] + sliceHeight[3], sliceWidth[6], sliceHeight[6], clip, angle, center, flip);
    drawSlice(renderer, 7, spriteInfo.stretchBottomCenter, x + sliceWidth[0], y + sliceHeight[0] + sliceHeight[3], sliceWidth[7], sliceHeight[7], clip, angle, center, flip);
    drawSlice(renderer, 8, spriteInfo.stretchBottomRight, x + sliceWidth[0] + sliceWidth[1], y + sliceHeight[0] + sliceHeight[3], sliceWidth[8], sliceHeight[8], clip, angle, center, flip);

    //Reset texture color
    spriteInfo.atlas->setColorMod(255, 255, 255);
    spriteInfo.atlas->setAlpha(255);
}

/// @brief Draws a nine-slice section.
/// @param renderer The renderer.
/// @param spriteIndex Index of atlas to draw.
/// @param drawStreched Whether to draw this slice as streched, or repeated.
/// @param x X draw location.
/// @param y Y draw location.
/// @param w Width to draw slice at.
/// @param h Height to draw slice at.
/// @param clip Portion of the texture to render for each slice.
/// @param angle Angle to rotate each slice.
/// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
/// @param flip Whether to flip each slice horizontally or vertically.
void NineSlice::drawSlice(SDL_Renderer *renderer, unsigned int spriteIndex, bool drawStreched, int x, int y, unsigned int w, unsigned int h, SDL_FRect *clip, double angle, SDL_FPoint *center, SDL_FlipMode flip) const
{
    if (w == 0 || h == 0)
    {
        return;
    }

    if (drawStreched)
    {
	    spriteInfo.atlas->renderStretched(renderer, spriteIndex, x, y, w, h, clip, angle, center, flip);
    }
    else
    {
	spriteInfo.atlas->renderRepeated(renderer, spriteIndex, x, y, w, h, clip, angle, center, flip);
    }
}

/// @brief Sets the rectangle size.
/// @param w Width in pixels.
/// @param h Height in pixels.
void NineSlice::setSize(unsigned int w, unsigned int h)
{
    width = w;
    height = h;
}

/// @return Rectangle width in pixels.
unsigned int NineSlice::getWidth() const
{
    return width;
}

/// @return Rectangle height in pixels.
unsigned int NineSlice::getHeight() const
{
    return height;
}

/// @return The base slice size. This is the sprite size in the atlas.
unsigned int NineSlice::getSliceSize() const
{
    return spriteSize;
}
