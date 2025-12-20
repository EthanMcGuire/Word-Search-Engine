#pragma once

#ifndef NINE_SLICE_H
#define NINE_SLICE_H

#include <SDL3/SDL_render.h>
#include <gmtl/gmtl.h>

class SpriteAtlas;

/*Sprite atlas sprites are assumed to be formated like so:
	0 1 2
	3 4 5
	6 7 8
*/

struct NineSliceSpriteInfo
{
	SpriteAtlas *atlas = NULL;	//Atlas that contains the nine-slice sprites
	bool stretchLeft = true;
	bool stretchTop = true;
	bool stretchRight = true;
	bool stretchMiddleLeft = true;
	bool stretchMiddleCenter = true;
	bool stretchMiddleRight = true;
	bool stretchBottomLeft = true;
	bool stretchBottomCenter = true;
	bool stretchBottomRight = true;
};

class NineSlice
{
    public:
        NineSlice();

        /// @brief Sets the nine slice sprite atlas and sprite indices.
        /// @param atlas The sprite atlas to use.
	void setSpriteInfo(NineSliceSpriteInfo spriteInfo);

        /// @brief Clears the sprite info.
        void clearSpriteInfo();

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
        void render(SDL_Renderer *renderer, int x, int y, SDL_FRect *clip = NULL, double angle = 0.0, SDL_FPoint *center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE, int alpha = 255, gmtl::Vec3i colorMod = {255, 255, 255}) const;

        /// @brief Draws a nine-slice section.
        /// @param renderer The renderer.
        /// @param spriteIndex Index of atlas to draw.
	/// @param drawStretched Whether to draw this slice as streched, or repeated.
        /// @param x X draw location.
        /// @param y Y draw location.
        /// @param w Width to draw slice at.
        /// @param h Height to draw slice at.
        /// @param clip Portion of the texture to render for each slice.
        /// @param angle Angle to rotate each slice.
        /// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
        /// @param flip Whether to flip each slice horizontally or vertically.
        void drawSlice(SDL_Renderer *renderer, unsigned int spriteIndex, bool drawStretched, int x, int y, unsigned int w, unsigned int h, SDL_FRect *clip, double angle, SDL_FPoint *center, SDL_FlipMode flip) const;
    
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
	NineSliceSpriteInfo spriteInfo;

        unsigned int spriteSize;    //Width/height for each sprite in the nine-slice
        unsigned int minSize;       //Minimum width/height in pixels
        unsigned int width;
        unsigned int height;
};

#endif
