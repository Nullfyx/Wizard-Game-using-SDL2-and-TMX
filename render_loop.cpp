
#include "render_loop.hpp"
#include "globals.hpp"
#include <SDL2/SDL_render.h>

bool renderLoop(const char *path)
{
    int jumpIndex = 0;
    SDL_Event e;
    bool quit = false;

    // Load map
    tmx_map *map = tmx_load(path);
    if (!map)
    {
        tmx_perror("cannot load map");
        return false;
    }

    bool moveLeft = false;
    bool moveRight = false;
    bool jump = false;

    // Player setup
    Player player;
    player.setMap(map);
    std::string src = "sprites/wizard/wizard_idle.png";
    std::string rightSrc = "sprites/wizard/wizard_run.png";
    std::string leftSrc = "sprites/wizard/wizard_run.png";

    player.playerTexture.setFPS(30);
    int cols = 5;
    int cells = 5;
    player.playerTexture.WIMG_Load(src);
    player.setWidth(16);
    player.setHeight(16);
    player.playerTexture.setCells(cells);
    player.playerTexture.setCols(cols);
    player.playerTexture.setFPS(4);

    // Load background texture ONCE before main loop
    SDL_Texture* backgroundTex = IMG_LoadTexture(wRenderer, "bg.png");
    if (!backgroundTex)
    {
        SDL_Log("Failed to load background texture: %s", SDL_GetError());
        return false;
    }

    int bgTexW, bgTexH;
    SDL_QueryTexture(backgroundTex, NULL, NULL, &bgTexW, &bgTexH);

    Timer frameTimer;
    frameTimer.start();
    float deltaTime = 0.0f;

    while (!quit)
    {
        // Jump logic using jumpIndex
        if (jumpIndex > 0)
        {
            jump = true;
            jumpIndex = (jumpIndex + 1) % 5;
        }
        else
        {
            jumpIndex = 0;
            jump = false;
        }

        deltaTime = frameTimer.getTicks() / 1000.0f;
        frameTimer.start();

        // Input handling
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;

            if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_LEFT:
                    moveLeft = true;
                    player.playerTexture.WIMG_Load(leftSrc);
                    player.setFlip(SDL_FLIP_HORIZONTAL);
                    player.playerTexture.setCells(4);
                    player.playerTexture.setCols(4);
                    break;

                case SDLK_RIGHT:
                    moveRight = true;
                    player.playerTexture.WIMG_Load(rightSrc);
                    player.setFlip(SDL_FLIP_NONE);
                    player.playerTexture.setCells(4);
                    player.playerTexture.setCols(4);
                    break;

                case SDLK_SPACE:
                    if (jumpIndex < 10)
                    {
                        jumpIndex = 1;
                        jump = true;
                    }
                    break;
                }
            }

            if (e.type == SDL_KEYUP && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_LEFT:
                    moveLeft = false;
                    player.setFlip(SDL_FLIP_NONE);
                    player.playerTexture.WIMG_Load(src);
                    player.playerTexture.setCells(5);
                    player.playerTexture.setCols(5);
                    break;

                case SDLK_RIGHT:
                    moveRight = false;
                    player.setFlip(SDL_FLIP_NONE);
                    player.playerTexture.WIMG_Load(src);
                    player.playerTexture.setCells(5);
                    player.playerTexture.setCols(5);
                    break;
                }
            }
        }

        // Update camera
        int map_width_px = map->width * map->tile_width;
        int map_height_px = map->height * map->tile_height;
        float scale = 4.0f;

        int scaledScreenWidth = SCREEN_WIDTH / scale;
        int scaledScreenHeight = SCREEN_HEIGHT / scale;

        if (!jump)
            camera.y = player.kyPos + player.height() / 2 - scaledScreenHeight / 2;

        camera.x = player.kxPos + player.width() / 2 - scaledScreenWidth / 2;

        // Clamp camera inside map bounds
        if (camera.x < 0)
            camera.x = 0;
        if (camera.y < 0)
            camera.y = 0;
        if (camera.x > map_width_px - camera.w)
            camera.x = map_width_px - camera.w;
        if (camera.y > map_height_px - camera.h)
            camera.y = map_height_px - camera.h;

        camera.w = scaledScreenWidth;
        camera.h = scaledScreenHeight;

        // Update game logic
        player.update(deltaTime);

        // Background rendering - full height, parallax horizontally
        float scaleBg = (float)SCREEN_HEIGHT / bgTexH;
        int finalBgWidth = (int)(bgTexW * scaleBg);
        int finalBgHeight = SCREEN_HEIGHT;

        SDL_Rect bgRect = {
            -camera.x / 2,  // horizontal parallax
            0,              // fixed vertical position for full height
            finalBgWidth,
            finalBgHeight
        };
        SDL_RenderCopy(wRenderer, backgroundTex, nullptr, &bgRect);

        // Render map and player
        render_map(map);
        player.moveRender(moveRight, moveLeft, jump);

        SDL_RenderPresent(wRenderer);

        // Cap frame rate (~60 FPS)
        Uint32 frameTicks = frameTimer.getTicks();
        if (frameTicks < 16)
            SDL_Delay(16 - frameTicks);
    }

    // Cleanup
    SDL_DestroyTexture(backgroundTex);
    tmx_map_free(map);

    return true;
}
