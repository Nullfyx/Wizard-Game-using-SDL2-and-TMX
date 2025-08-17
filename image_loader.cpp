#include "image_loader.hpp"
extern "C" void *tmx_load_texture(const char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    std::cout << "Loading tileset image: " << path << std::endl;

    if (!surface)
    {
        std::cerr << "IMG_Load failed: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(wRenderer, surface);
    SDL_FreeSurface(surface);

    if (!texture)
    {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        return nullptr;
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    return texture;
}

extern "C" void tmx_free_texture(void *ptr)
{
    SDL_DestroyTexture(static_cast<SDL_Texture *>(ptr));
}
