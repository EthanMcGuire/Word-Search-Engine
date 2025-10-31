#pragma once

#ifndef SPRITE_ATLAS_H
#define SPRITE_ATLAS_H

#include "texture.hpp"

class SpriteAtlas : public Texture
{
    public:
    
        SpriteAtlas();
        ~SpriteAtlas();

        /// @brief Loads the given image file as a sprite sheet.
        /// @param renderer The renderer the texture will be created for.
        /// @param path Image file path.
        /// @param spriteFrameWidth The width of individual sprites on the sprite sheet.
        /// @param spriteFrameHeight The height of individual sprites on the sprite sheet.
        /// @return True on success.
        bool loadTexture(SDL_Renderer *renderer, std::string path, int spriteFrameWidth, int spriteFrameHeight);

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
        /// @param flip Whether to flip the texture horizontally or vertically.
        void render(SDL_Renderer *renderer, unsigned int spriteIndex, int x, int y, double xScale = 1.0, double yScale = 1.0, SDL_FRect *clip = NULL, double angle = 0.0, SDL_FPoint *center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE) const;

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
        /// @param flip Whether to flip the texture horizontally or vertically.
        void renderStretched(SDL_Renderer *renderer, unsigned int spriteIndex, int x, int y, int drawWidth, int drawHeight, SDL_FRect *clip = NULL, double angle = 0.0, SDL_FPoint *center = NULL, SDL_FlipMode flip = SDL_FLIP_NONE) const;

        #pragma region Getters

        unsigned int getSpriteCount() const;
        unsigned int getColumnCount() const;
        unsigned int getRowCount() const;
        int getSpriteWidth() const;
        int getSpriteHeight() const;

        #pragma endregion Getters

    private:

        /// @brief Clears the sprite atlas variables.
        void clearAtlasInfo();

        //Sprite count info
        unsigned int spriteCount;
        unsigned int columnCount, rowCount;

        //Individual sprite sizes
        int spriteWidth, spriteHeight;
};

#endif