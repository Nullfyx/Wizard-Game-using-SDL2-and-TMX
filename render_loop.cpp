#include "render_loop.hpp"
bool renderLoop(const char *path)
{
    SDL_Event e;
    bool quit = false;
    // render the map
    tmx_map *map = tmx_load(path);
    if (!map)
    {
        tmx_perror("cannot load map");
        return false;
    }
    // Player setup
    Player player;
    player.setMap(map);
    string src = "sprites/wizard/wiz.png";
    string rightSrc = "sprites/wizard/wiz_move.png";
    string leftSrc = "sprites/wizard/wiz_move_flip.png";
    int cols = 6;
    int cells = 6;
    player.playerTexture.WIMG_Load(src);
    player.setYPos(0);
    player.gravity.setG(1);
    player.setWidth(64);
    player.setHeight(64);
    player.playerTexture.setCells(cells);
    player.playerTexture.setCols(cols);
    player.playerTexture.setFPS(60);
    // Timer for deltaTime tracking
    Timer frameTimer;
    frameTimer.start();
    float deltaTime = 0.0f;

    while (!quit)
    {

        deltaTime = frameTimer.getTicks() / 1000.0f;
        frameTimer.start();
        float jumpImpulse = -10;

        // Input handling
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;

            if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_RIGHT:
                    player.setXVel(player.xVel() + player.maxVel());
                    player.setRotate(5);
                    player.playerTexture.WIMG_Load(rightSrc);
                    break;
                case SDLK_LEFT:
                    player.setXVel(player.xVel() - player.maxVel());
                    player.setRotate(-5);
                    player.playerTexture.WIMG_Load(leftSrc);
                    break;
                case SDLK_SPACE:
                    cout << "SPACE0" << endl;
                    if (player.getUpVel())
                    {
                        player.jump();
                    }
                    break;
                }
            }

            if (e.type == SDL_KEYUP && e.key.repeat == 0)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_RIGHT:
                    player.setXVel(player.xVel() - player.maxVel());
                    player.setRotate(0);
                    player.playerTexture.WIMG_Load(src);
                    break;
                case SDLK_LEFT:
                    player.setXVel(player.xVel() + player.maxVel());
                    player.setRotate(0);
                    player.playerTexture.WIMG_Load(src);
                    break;
                }
            }
            // if ((e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) && e.key.repeat == 1)
            // {
            //     switch (e.key.keysym.sym)
            //     {
            //     case SDLK_SPACE:
            //         player.setYVel(player.gravityVel());
            //     }
            // }
        }
        // update camera
        int map_width_px = map->width * map->tile_width;
        int map_height_px = map->height * map->tile_height;
        float scale = 2.0f; 
        cout << player.yVel() << endl;
        // Compute scaled screen dimensions
        int scaledScreenWidth = SCREEN_WIDTH / scale;
        int scaledScreenHeight = SCREEN_HEIGHT / scale;

        // Update camera
        camera.x = player.xPos() + player.width() / 2 - scaledScreenWidth / 2;
        camera.y = player.yPos() + player.height() / 2 - scaledScreenHeight / 2;

        // Clamp camera inside map
        if (camera.x < 0)
            camera.x = 0;
        if (camera.y < 0)
            camera.y = 0;
        if (camera.x > map_width_px - camera.w)
            camera.x = map_width_px - camera.w;
        if (camera.y > map_height_px - camera.h)
            camera.y = map_height_px - camera.h;

        // Also update camera's width/height to match scaled view
        camera.w = scaledScreenWidth;
        camera.h = scaledScreenHeight;

        // Update game logic
        player.update(deltaTime);
        // Rendering
        SDL_SetRenderDrawColor(wRenderer, 100, 0, 150, 255);
        SDL_RenderClear(wRenderer);
        render_map(map);
        player.moveRender();
        player.isTileCollidable();
        SDL_RenderPresent(wRenderer);

        // Cap frame rate at ~60 FPS
        Uint32 frameTicks = frameTimer.getTicks();
        SDL_Delay(16);
    }

    return true;
}
