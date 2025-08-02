#include "texture.hpp"
#include "timer.hpp"
// constructor
WTexture::WTexture()
{
    // clear garbage values
    wTexture = NULL;
    wWidth = 0;
    wHeight = 0;
}

// remove the texture
void WTexture::free()
{
    // assign null to texture
    wTexture = NULL;

    // reset dimensions
    wHeight = 0;
    wWidth = 0;
}

// image load to texture
void WTexture::WIMG_Load(string src)
{
    // free any pre existing texture
    free();

    // generate a temporary surface to load image
    SDL_Surface *loadedImage = IMG_Load(src.c_str());

    // if error
    if (!loadedImage)
    {
        cout << "Error loading image: " << src << " SDL ERR: " << IMG_GetError() << endl;
        return;
    }

    // generate a temporary texture to load image
    SDL_Texture *loadedTexture = SDL_CreateTextureFromSurface(wRenderer, loadedImage);

    // if error
    if (!loadedTexture)
    {
        cout << "Error Generating Texture, SDL ERR: " << IMG_GetError() << endl;
        return;
    }

    // assign the texture
    wTexture = loadedTexture;

    // set the dimensions
    SDL_QueryTexture(loadedTexture, NULL, NULL, &this->wWidth, &this->wHeight);
    // clean
    SDL_FreeSurface(loadedImage);
}

// text to texture
void WTexture::textTex(string str, TTF_Font *wFont, SDL_Color wColor, SDL_Renderer *wRenderer)
{
    // if the font variable is NULL
    if (!wFont)
    {
        cout << "Error loading fonts" << endl;
        return;
    }

    // free preexisting font
    free();

    // write text to surface
    SDL_Surface *loadedSurface = TTF_RenderText_Blended(wFont, str.c_str(), wColor);

    // if error
    if (!loadedSurface)
    {
        cout << "Error writing text to surface, SDL ERR: " << TTF_GetError() << endl;
        return;
    }

    // create texture from surface
    SDL_Texture *loadedTexture = SDL_CreateTextureFromSurface(wRenderer, loadedSurface);

    // if error
    if (!loadedTexture)
    {
        cout << "Error generating texture from text: " << str << " SDL ERR: " << SDL_GetError() << endl;
        return;
    }

    // assign texture and dimensions
    wTexture = loadedTexture;
    SDL_QueryTexture(loadedTexture, NULL, NULL, &this->wWidth, &this->wHeight);
}

// animate the sprite
void WTexture::animateSprite(SDL_Renderer *wRenderer, int cols, int cells, SDL_Rect destRect, double angle, SDL_Point *center, SDL_RendererFlip flip)
{
    static int frame = 0;
    static Timer animationTimer;

    static bool timerStarted = false;
    if (!timerStarted)
    {
        animationTimer.start();
        timerStarted = true;
    }

    Uint32 frameDelay = 1000 / fps;

    if (animationTimer.getTicks() >= frameDelay)
    {
        frame = (frame + 1) % cells;
        animationTimer.start(); // Reset the timer
    }

    int rows = cells / cols;
    int frameWidth = wWidth / cols;
    int frameHeight = wHeight / rows;

    SDL_Rect srcRect;
    srcRect.x = (frame % cols) * frameWidth;
    srcRect.y = (frame / cols) * frameHeight;
    srcRect.w = frameWidth;
    srcRect.h = frameHeight;

    render(wRenderer, srcRect, destRect, angle, center, flip);
}

// render the texture
void WTexture::render(SDL_Renderer *renderer, SDL_Rect srcRect, SDL_Rect destRect, double angle, SDL_Point *center, SDL_RendererFlip flip)
{
    if (angle != 0.0 || center != NULL || flip != SDL_FLIP_NONE)
    {
        // Use extended function if rotation or flip is involved
        SDL_RenderCopyEx(renderer, wTexture, &srcRect, &destRect, angle, center, flip);
    }
    else
    {
        // Use simple render
        SDL_RenderCopy(renderer, wTexture, &srcRect, &destRect);
    }
}

// selected setters and getters
void WTexture::setCols(int c)
{
    wCols = c;
}

void WTexture::setCells(int c)
{
    wCells = c;
}

void WTexture::setFPS(int f)
{
    fps = f;
}

int WTexture::getCols()
{
    return wCols;
}

int WTexture::getCells()
{
    return wCells;
}

int WTexture::getFPS()
{
    return fps;
}
