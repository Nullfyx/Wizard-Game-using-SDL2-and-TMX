#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include "globals.hpp"

using namespace std;

class WTexture
{
    // our actual hardware accelerated texture
    SDL_Texture *wTexture;

    // dimensions of the texture
    int wHeight;
    int wWidth;

    // spritesheet data
    int wCols;
    int wCells;
    static int fps;
public:
    // constructor
    WTexture();
    int angle;
    // remove the texture
    void free();

    // image load to texture
    void WIMG_Load(string src);

    // text to texture
    void textTex(string str, TTF_Font *wFont, SDL_Color wColor, SDL_Renderer *wRenderer);

    // animated sprite
    void animateSprite(SDL_Renderer *wRenderer, int cols, int cells, SDL_Rect *destRect, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    // render the texture
    void render(SDL_Renderer *renderer, SDL_Rect *srcRect, SDL_Rect *destRect, double angle = 0.0, SDL_Point *center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

    void setColor( Uint8 red, Uint8 green, Uint8 blue );

        //Set blending
        void setBlendMode( SDL_BlendMode blending );

        //Set alpha modulation
        void setAlpha( Uint8 alpha );
    // selected setters and getters
    void setCols(int c);

    void setCells(int c);

    void setFPS(int f);

    int getCols();

    int getCells();

    int getFPS();

    void readAlpha(Uint8 &a);

};
#endif
