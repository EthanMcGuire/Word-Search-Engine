#include "drawingSurface.hpp"
#include <SDL_log.h>

/// @brief Creates a drawing surface to draw on.
DrawingSurface::DrawingSurface()
{
    texture = NULL;
}

DrawingSurface::~DrawingSurface()
{
    freeSurface();
}

/// @brief Creates the drawing surface. Can be called again to resize the surface.
/// @param w Surface width.
/// @param h Surface height.
/// @return True on success, false otherwise.
bool DrawingSurface::createSurface(SDL_Renderer *renderer, unsigned int w, unsigned int h)
{
    freeSurface();

    width = w;
    height = h;
    
    texture = SDL_CreateTexture(renderer, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET, w, h);

    if (texture == NULL)
    {
        width = 0;
        height = 0;

        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "DrawingSurface: Failed to create drawing surface: %s", SDL_GetError());

        return false;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "DrawingSurface: Surface created: W = %d, H = %d", width, height);

    return true;
}

/// @brief Frees the drawing surface from memory.
void DrawingSurface::freeSurface()
{
    if (texture == NULL)
    {
        return;
    }

    SDL_DestroyTexture(texture);
    texture = NULL;

    width = 0;
    height = 0;

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "DrawingSurface: Surface freed");
}

/// @brief Checks if the surface texture exists.
/// @return True if the surface texture was created, false otherwise.
bool DrawingSurface::surfaceExists() const
{
    return texture != NULL;    
}

/// @brief Sets the rendering target to this drawing surface.
/// @param renderer The renderer to set the target for.
void DrawingSurface::targetSurface(SDL_Renderer *renderer) const
{
    if (!surfaceExists())
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "DrawingSurface: Failed to target the drawing surface. Create the surface first with createSurface()");

        return;
    }

    SDL_SetRenderTarget(renderer, texture);
}

/// @brief Draws the surface.
/// @param renderer The renderer to draw to.
/// @param x X draw position.
/// @param y Y draw position.
/// @param xScale The X scale to draw the surface at.
/// @param yScale The Y scale to draw the surface at.
/// @param clip Portion of the texture to render. NULL to render the full texture.
/// @param angle Image rotation angle.
/// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
/// @param flip Whether to flip the texture horizontally or vertically.
void DrawingSurface::drawSurface(SDL_Renderer *renderer, int x, int y, double xScale, double yScale, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip) const
{
    if (!surfaceExists())
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "DrawingSurface: Failed to draw the drawing surface. Create the surface first with createSurface()");

        return;
    }

    SDL_Rect renderQuad = {x, y, (int) width, (int) height};

    if (clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Apply scale
    renderQuad.w *= xScale;
    renderQuad.h *= yScale;

    //Render the texture
    SDL_RenderCopyEx(renderer, texture, clip, &renderQuad, angle, center, flip);
}

/// @brief Draws the surface.
/// @param renderer The renderer to draw to.
/// @param x X draw position.
/// @param y Y draw position.
/// @param drawWidth Width to draw the texture at.
/// @param drawHeight Height to draw the texture at.
/// @param clip Portion of the texture to render. NULL to render the full texture.
/// @param angle Image rotation angle.
/// @param center Center point to rotate the texture at. Defaults at w/2, h/2 of the dest rect (image center).
/// @param flip Whether to flip the texture horizontally or vertically.
void DrawingSurface::drawSurfaceStretched(SDL_Renderer *renderer, int x, int y, int drawWidth, int drawHeight, SDL_Rect *clip, double angle, SDL_Point *center, SDL_RendererFlip flip) const
{
    if (!surfaceExists())
    {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "DrawingSurface: Failed to draw the drawing surface. Create the surface first with createSurface()");

        return;
    }

    SDL_Rect renderQuad = {x, y, drawWidth, drawHeight};

    //Render the texture
    SDL_RenderCopyEx(renderer, texture, clip, &renderQuad, angle, center, flip);
}

/// @brief Set the color modulation (multiplier) for the surface.
/// @param r Red component (0 - 255).
/// @param g Green component (0 - 255).
/// @param b Blue component (0 - 255).
void DrawingSurface::setColorMod(Uint8 r, Uint8 g, Uint8 b)
{
    if (!surfaceExists())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "DrawingSurface: Failed to set color modulation for the drawing surface. Create the surface first with createSurface()");

        return;
    }

    SDL_SetTextureColorMod(texture, r, g, b);
}

/// @brief Sets the blend mode for the surface.
/// @param blending The blend mode.
void DrawingSurface::setBlendMode(SDL_BlendMode blending)
{
    if (!surfaceExists())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "DrawingSurface: Failed to set blend mode for the drawing surface. Create the surface first with createSurface()");

        return;
    }

    SDL_SetTextureBlendMode(texture, blending);
}

/// @brief Sets the alpha for the surface.
/// @param alpha Alpha, from 0 to 255.
void DrawingSurface::setAlpha(Uint8 alpha)
{
    if (!surfaceExists())
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "DrawingSurface: Failed to set alpha for the drawing surface. Create the surface first with createSurface()");

        return;
    }

    SDL_SetTextureAlphaMod(texture, alpha);
}

/// @return The surface width in pixels.
unsigned int DrawingSurface::getWidth() const
{
    return width;
}

/// @return The surface height in pixels.
unsigned int DrawingSurface::getHeight() const
{
    return height;
}