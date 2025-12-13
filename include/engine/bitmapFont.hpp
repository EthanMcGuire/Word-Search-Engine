#pragma once

#ifndef BITMAP_FONT_H
#define BITMAP_FONT_H

#include "texture.hpp"
#include "rapidxml/rapidxml.hpp"
#include <string>
#include <vector>
#include <unordered_map>

using namespace rapidxml;

class Random;

struct CharInfo
{
    int id;
    int x;          //The x position of the top left of the quad in the texture.
    int y;          //The y position of the top left of the quad in the texture.
    int width;      //The width of the quad to render.
    int height;     //The height of the quad to render. 
    int xoffset;    //The offset from the left of the cell to the left of the character. This is the x postion where the character should be drawn.
    int yoffset;    //The offset from the top of the cell to the top of the character
                        //This can be used in combination with base, to figure out where to draw the character vertically.
                        //Drawing at yoffset sets the characters base to be at (yoffset + height). Shift the drawing y location by y + (base - (yoffset + height))
    int xadvance;   //Amount to move the char cursor horizontally after this character (Char width)
    int page;
    int channel;    //We dont need to worry about channel, since we use all channels currently 
};

enum TextAlign
{
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    CENTER
};

/// @brief This class is used to load and render bitmap fonts, created from converting a TTF font to a bitmap font using bmfont.
///        The fonts texture pages are expected to be white characters on a transparent background.
///
///        Escape characters can be used for special effects when rendering the font. / is used instead of \ (Since \ is used by C++ compiler.)
class BitmapFont
{
    public:
        /// @brief Creates a bitmap font class.
        /// @param random The engines random number generator. Used for character shake.
        BitmapFont(Random *random);
        ~BitmapFont();

        /// @brief Loads the font. This is a wrapper for the loadFontFromXml() method.
        /// @param renderer The render to load the texture for.
        /// @param path The path to the font configuration file.
        /// @return True on success, false otherwise.
        bool loadFont(SDL_Renderer *renderer, std::string path);

        /// @brief Loads the font from the XML file.
        /// @param renderer The render to load the texture for.
        /// @param path The path to the font configuration file.
        /// @return True on success, false otherwise.
        bool loadFontFromXml(SDL_Renderer *renderer, std::string path);

        /// @brief Frees the current font texture and resets the configuration.
        void freeFont();

        /// @brief Draws text to the screen.
        /// @param renderer The renderer to draw to.
        /// @param x The x position to draw at. Relative to the viewport.
        /// @param y The y position to draw at. Relative to the viewport.
        /// @param text The text to draw.
        /// @param color The text color. Default is white.
        void drawText(SDL_Renderer *renderer, int x, int y, std::string text, SDL_Color color = {255, 255, 255, 255});     

        void drawTextOutlined(SDL_Renderer *renderer, int x, int y, std::string text, SDL_Color color = {255, 255, 255, 255}, SDL_Color outlineColor = {0, 0, 0, 255}, TextAlign hAlign = LEFT, TextAlign vAlign = TOP);     
	
        /// @brief Draws the given text with line width and separation.
        /// @param renderer The renderer to draw to.
        /// @param x The x position to draw at. Relative to the viewport.
        /// @param y The y position to draw at. Relative to the viewport.
        /// @param text The text to draw.
        /// @param lineWidth Line width. If -1, is infinite.
        /// @param sep Line separation. If -1, uses the default line height.
        /// @param color The text color. Default is white.
        void drawTextExt(SDL_Renderer *renderer, int x, int y, std::string text, int lineWidth = -1, int sep = -1, SDL_Color color = {255, 255, 255, 255});

        void drawTextAligned(SDL_Renderer *renderer, int x, int y, std::string text, SDL_Color color = {255, 255, 255, 255}, TextAlign hAlign = LEFT, TextAlign vAlign = TOP);
        void drawTextAlignedExt(SDL_Renderer *renderer, int x, int y, std::string text, int lineWidth = -1, int sep = -1, SDL_Color color = {255, 255, 255, 255}, TextAlign hAlign = LEFT, TextAlign vAlign = TOP);

        /// @param text The text to check the width of.
        /// @param lineWidth Line width. If -1, is infinite.
        /// @param sep Line separation. If -1, uses the default line height.
        /// @return The text width
        unsigned int getTextWidth(std::string text, int lineWidth = -1, int sep = -1);

        /// @param text The text to check the height of.
        /// @param lineWidth Line width. If -1, is infinite.
        /// @param sep Line separation. If -1, uses the default line height.
        /// @return The text height
        unsigned int getTextHeight(std::string text, int lineWidth = -1, int sep = -1);

        /// @brief Gets the X and Y offset to draw text based on H and V alignment.
        /// @param xOffset X offset to be set.
        /// @param yOffset Y offset to be set.
        /// @param text The text to get offset from.
        /// @param lineWidth Line width. If -1, is infinite.
        /// @param sep Line separation. If -1, uses the default line height.
        /// @param hAlign Horizontal alignment.
        /// @param vAlign Vertical alignment.
        void getTextAlignOffset(int &xOffset, int &yOffset, std::string text, int lineWidth = -1, int sep = -1, TextAlign hAlign = LEFT, TextAlign vAlign = TOP);

	unsigned int getCharWidth(char c);
	unsigned int getCharHeight(char c);

        //Figure out how to render the outline color and text color separately
            //THIS IS ONLY IF THE BITMAP FONT WAS GENERATED WITH A "baked outline"

    private:  
        /// @brief Gets the width and height of the given text for this font.
        /// @param width The returned width.
        /// @param height The returned height.
        /// @param minYOffset The returned smallest Y Offset for the first line in relation to the drawing position.
        /// @param text The text to get the size of.
        /// @param lineWidth Line width. If -1, is infinite.
        /// @param sep Line separation. If -1, uses the default line height.
        void getTextSize(unsigned int &width, unsigned int &height, int &minYOffset, std::string text, int lineWidth = -1, int sep = -1);

        /// @brief Gets the next words width in the current text.
        /// @param text The text to get the word width from.
        /// @param charIndex The index to start from.
        /// @return The word width.
        int getNextWordWidth(std::string text, int charIndex);

        /// @brief Reads the attributes string value if it exists. 
        /// @param node The node to get an attribute for.
        /// @param attrName The attribute name.
        /// @return The attribute value, or an empty string if it doesn't exist.
        std::string getAttrValue(xml_node<> *node, std::string attrName);

        /// @brief Draws a character to the screen using the current cursor position.
        /// @param renderer The renderer to draw to.
        /// @param charInfo The font info on the character to draw.
        void drawChar(SDL_Renderer *renderer, CharInfo *charInfo);

        /// @brief Handles a special character.
        /// @param specialChar The character.
        /// @return True if the current characters rendering should be skipped.
        bool handleSpecialChar(char specialChar);

        /// @param c The character to get the info for.
        /// @return The fonts CharInfo for the given character, or NULL if the character isn't in the font.
        CharInfo* getCharInfo(char c);

        /// @brief Sets a new draw color/alpha if the color or alpha was changed.
        /// @param color The draw color to set.
        void setDrawColor(SDL_Color color);

        /// @brief Sets the alpha for the font.
        /// @param alpha Alpha, from 0 to 255.
        void setAlpha(Uint8 alpha);

        Random *rng;

        bool fontLoaded;

        //The font texture pages
        std::vector<Texture*> texturePages;     

        //Drawing info
        int cursorX, cursorY;   //The drawing cursor position
        int baseCursorX;        //The intial drawing x position
        int currentSep;         //The current line separation (line height)
        SDL_Color drawColor;

        //Shake
        int shakeIntensity;     //The +- shake intensity
        bool shake;             //Whether we should shake characters

        /*Font configuration*/

        /// *info*
        //Info on how the font was generated
        std::string fontName;
        

        //Maybe I'll use these later
        /*
        std::string charset;    //The name of the OEM charset (If not unicode)
        bool isUnicode;         //Whether the font is unicode characters
        int outlineThickness;   //Font outline pixel thickness

        int size;
        bool isBold;
        bool isItalic;
        bool isSmooth;      //Whether smoothing was turned on

        int stretchH;       //Font height size stretch in percentage. 100 means no stretch.
        int aa;             //The supersampling level used (anti-aliasing). 1 means no supersampling was used. (SSAA -> super sampling anti aliasing)

        int paddingLeft, paddingRight, paddingUp, paddingDown;  //Padding for each character
        int spacingHorizontal, spacingVertical;                 //Spacing for each character
        */

        /// *common*
        //Information common to all characters
        int lineHeight;         //Line separation between each line of text
        int base;               //Number of pixels from the (absolute/highest) top of the line to the base of the characters
        int scaleW, scaleH;     //Width and height of the texture
        unsigned int pageCount;          //The number of texture pages
        bool packed;            //True if the monochrome characters have been packed into each of the texture channels
        
        int redChnl;            //Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyph and the outline, 3 if its set to zero, and 4 if its set to one.
        int greenChnl;          //Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyph and the outline, 3 if its set to zero, and 4 if its set to one.
        int blueChnl;           //Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyph and the outline, 3 if its set to zero, and 4 if its set to one.
        int alphaChnl;          //Set to 0 if the channel holds the glyph data, 1 if it holds the outline, 2 if it holds the glyph and the outline, 3 if its set to zero, and 4 if its set to one.

        /// *chars*
        //The character metadata
        std::unordered_map<int, CharInfo> chars;
        unsigned int charCount;
};

#endif
