#include "nineSlice.hpp"
#include "spriteAtlas.hpp"
#include <algorithm>

NineSlice::NineSlice()
{
    scaleMode = NINE_SLICE_SCALE_MODE_STRETCH;

    atlas = nullptr;

    width = 0;
    height = 0;

    for (int i = 0; i < 9; ++i) 
    {
        spriteIndices[i] = 0;
    }
}

/// @brief Sets the nine slice sprite atlas and sprite indices.
/// @param atlas The sprite atlas to use.
/// @param topLeft Top left sprite index in the atlas.
/// @param topCenter Top center sprite index in the atlas.
/// @param topRight Top right sprite index in the atlas.
/// @param middleLeft Middle left sprite index in the atlas.
/// @param middleCenter Middle center sprite index in the atlas.
/// @param middleRight Middle right sprite index in the atlas.
/// @param bottomLeft Bottom left sprite index in the atlas.
/// @param bottomCenter Bottom center sprite index in the atlas.
/// @param bottomRight Bottom right sprite index in the atlas.
void NineSlice::setSprites(SpriteAtlas *atlas, unsigned int topLeft, unsigned int topCenter, unsigned int topRight, unsigned int middleLeft, unsigned int middleCenter, 
                                    unsigned int middleRight, unsigned int bottomLeft, unsigned int bottomCenter, unsigned int bottomRight)
{
    clearSprites();

    //Do nothing if the atlas is null
    if (atlas == nullptr)
    {
        return;
    }

    this->atlas = atlas;
    this->spriteSize = atlas->getSpriteWidth();
    minSize = spriteSize * 2;

    spriteIndices[0] = topLeft;
    spriteIndices[1] = topCenter;
    spriteIndices[2] = topRight;
    spriteIndices[3] = middleLeft;
    spriteIndices[4] = middleCenter;
    spriteIndices[5] = middleRight;
    spriteIndices[6] = bottomLeft;
    spriteIndices[7] = bottomCenter;
    spriteIndices[8] = bottomRight;
}

/// @brief Clears the sprite atlas.
void NineSlice::clearSprites()
{
    atlas = nullptr;
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
    if (atlas == nullptr || width == 0 || height == 0 || spriteSize == 0)
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
    atlas->setColorMod(colorMod[0], colorMod[1], colorMod[2]);
    atlas->setAlpha(alpha);

    //Draw the nine-slice
    drawSlice(renderer, spriteIndices[0], x, y, sliceWidth[0], sliceHeight[0], clip, angle, center, flip);
    drawSlice(renderer, spriteIndices[1], x + sliceWidth[0], y, sliceWidth[1], sliceHeight[1], clip, angle, center, flip);
    drawSlice(renderer, spriteIndices[2], x + sliceWidth[0] + sliceWidth[1], y, sliceWidth[2], sliceHeight[2], clip, angle, center, flip);
    drawSlice(renderer, spriteIndices[3], x, y + sliceHeight[0], sliceWidth[3], sliceHeight[3], clip, angle, center, flip);
    drawSlice(renderer, spriteIndices[4], x + sliceWidth[0], y + sliceHeight[0], sliceWidth[4], sliceHeight[4], clip, angle, center, flip);
    drawSlice(renderer, spriteIndices[5], x + sliceWidth[0] + sliceWidth[1], y + sliceHeight[0], sliceWidth[5], sliceHeight[5], clip, angle, center, flip);
    drawSlice(renderer, spriteIndices[6], x, y + sliceHeight[0] + sliceHeight[3], sliceWidth[6], sliceHeight[6], clip, angle, center, flip);
    drawSlice(renderer, spriteIndices[7], x + sliceWidth[0], y + sliceHeight[0] + sliceHeight[3], sliceWidth[7], sliceHeight[7], clip, angle, center, flip);
    drawSlice(renderer, spriteIndices[8], x + sliceWidth[0] + sliceWidth[1], y + sliceHeight[0] + sliceHeight[3], sliceWidth[8], sliceHeight[8], clip, angle, center, flip);

    //Reset texture color
    atlas->setColorMod(255, 255, 255);
    atlas->setAlpha(255);
}

/// @brief Draws a nine-slice section.
/// @param renderer The renderer.
/// @param spriteIndex Index of atlas to draw.
/// @param x X draw location.
/// @param y Y draw location.
/// @param w Width to draw slice at.
/// @param h Height to draw slice at.
/// @param clip Portion of the texture to render for each slice.
/// @param angle Angle to rotate each slice.
/// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
/// @param flip Whether to flip each slice horizontally or vertically.
void NineSlice::drawSlice(SDL_Renderer *renderer, unsigned int spriteIndex, int x, int y, unsigned int w, unsigned int h, SDL_FRect *clip, double angle, SDL_FPoint *center, SDL_FlipMode flip) const
{
    if (w == 0 || h == 0)
    {
        return;
    }

    atlas->renderStretched(renderer, spriteIndex, x, y, w, h, clip, angle, center, flip);
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
