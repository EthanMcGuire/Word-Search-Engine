#pragma once

#ifndef NINE_SLICE_H
#define NINE_SLICE_H

#include <SDL_render.h>
#include <gmtl/gmtl.h>

class SpriteAtlas;

//TODO
enum NineSliceScaleMode
{
    NINE_SLICE_SCALE_MODE_STRETCH,
    NINE_SLICE_SCALE_MODE_REPEAT
};

class NineSlice
{
    public:
        NineSlice();

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
        void setSprites(SpriteAtlas *atlas, unsigned int topLeft, unsigned int topCenter, unsigned int topRight, unsigned int middleLeft, unsigned int middleCenter, 
                                    unsigned int middleRight, unsigned int bottomLeft, unsigned int bottomCenter, unsigned int bottomRight);

        /// @brief Clears the sprite atlas.
        void clearSprites();

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
        void render(SDL_Renderer *renderer, int x, int y, SDL_Rect *clip = NULL, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, int alpha = 255, gmtl::Vec3i colorMod = {255, 255, 255}) const;

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
        void drawSlice(SDL_Renderer *renderer, unsigned int spriteIndex, int x, int y, unsigned int w, unsigned int h, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip) const;
    
        /// @brief Sets the rectangle size.
        /// @param w Width in pixels.
        /// @param h Height in pixels.
        void setSize(unsigned int w, unsigned int h);

        /// @return Rectangle width in pixels.
        unsigned int getWidth() const;

        /// @return Rectangle height in pixels.
        unsigned int getHeight() const;

        /// @return The base slice size. This is the sprite size in the atlas.
        unsigned int getSliceSize() const;

    private:
        NineSliceScaleMode scaleMode;

        SpriteAtlas *atlas;     //Atlas that contains the nine-slice sprites

        unsigned int spriteIndices[9];    //Nine-slice sprite indices in the atlas:
                                                        //0 1 2
                                                        //3 4 5
                                                        //6 7 8

        unsigned int spriteSize;    //Width/height for each sprite in the nine-slice
        unsigned int minSize;       //Minimum width/height in pixels
        unsigned int width;
        unsigned int height;
};

#endif