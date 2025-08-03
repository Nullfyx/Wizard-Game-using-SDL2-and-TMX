#include "render_loop.hpp"
bool renderLoop(const char *path)
{
    int jumpIndex = 0;
    SDL_Event e;
    bool quit = false;
    // render the map
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
    string src = "sprites/wizard/wiz.png";
    string rightSrc = "sprites/wizard/wiz_move.png";
    string leftSrc = "sprites/wizard/wiz_move_flip.png";
    int cols = 6;
    int cells = 6;
    player.playerTexture.WIMG_Load(src);
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
        cout << "========================" << endl;
        cout << "jumpindex: " << jumpIndex << endl;
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

        cout << player.yVel() << ' ' << jump << endl;
        deltaTime = frameTimer.getTicks() / 1000.0f;
        frameTimer.start();
        // Input handling
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;

            if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            {
                if (e.key.keysym.sym == SDLK_LEFT)
                    moveLeft = true;
                if (e.key.keysym.sym == SDLK_RIGHT)
                    moveRight = true;
                if ((e.key.keysym.sym == SDLK_SPACE) && (jumpIndex < 10))
                {
                    jumpIndex = 1;
                    jump = true;
                }
            }

            if (e.type == SDL_KEYUP && e.key.repeat == 0)
            {
                if (e.key.keysym.sym == SDLK_LEFT)
                    moveLeft = false;
                if (e.key.keysym.sym == SDLK_RIGHT)
                    moveRight = false;
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
        // Compute scaled screen dimensions
        int scaledScreenWidth = SCREEN_WIDTH / scale;
        int scaledScreenHeight = SCREEN_HEIGHT / scale;

        // Update camera
        if (!jump)
        {
            camera.y = player.kyPos + player.height() / 2 - scaledScreenHeight / 2;
        }
        camera.x = player.kxPos + player.width() / 2 - scaledScreenWidth / 2;

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
        player.moveRender(moveRight, moveLeft, jump);
        player.isTileCollidable();
        SDL_RenderPresent(wRenderer);

        // Cap frame rate at ~60 FPS
        Uint32 frameTicks = frameTimer.getTicks();
        SDL_Delay(16);
    }

    return true;
}
