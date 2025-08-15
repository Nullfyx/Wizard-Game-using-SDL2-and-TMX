
#include "render_loop.hpp"
#include "globals.hpp"
#include "map.hpp"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include"mapglobal.hpp"
#include "projectile.hpp"
#include "bounding_box.hpp"
#include <vector>
    vector<projectile*> projectiles = {};

static inline void draw_moving_tile(tmx_map *map, moving_tile *m) {
    if (!map || !m) return;
    unsigned int base_gid = m->ts_firstgid + m->base_local_id;
    tmx_tile *base_tile = tmx_get_tile(map, base_gid);
    if (!base_tile) return;

    unsigned int local_frame_id = (base_tile->animation_len > 0)
        ? base_tile->animation[m->anim.current_frame].tile_id
        : m->base_local_id;

    unsigned int current_gid = m->ts_firstgid + local_frame_id;
    tmx_tile *frame = tmx_get_tile(map, current_gid);
    if (!frame) return;

    tmx_tileset *ts = frame->tileset;
    tmx_image *im = frame->image ? frame->image : ts->image;
    void *image = im->resource_image;

    unsigned int sx = frame->ul_x;
    unsigned int sy = frame->ul_y;
    unsigned int sw = ts->tile_width;
    unsigned int sh = ts->tile_height;

    draw_tile(image, sx, sy, sw, sh, (unsigned int)m->x, (unsigned int)m->y, 1.0f, 0);
}
bool renderLoop(const char *path)
{
    int jumpIndex = 0;
    SDL_Event e;
    bool quit = false;

    // Load map
    map = tmx_load(path);
    if (!map) {
        tmx_perror("cannot load map");
        return false;
    }
    build_enemies_from_map(map);
    bool moveLeft = false;
    bool moveRight = false;
    bool jump = false;
    bool attack = false;
    // Player setup
    Player player;
    player.setMap(map);
    std::string src = "sprites/wizard/wizard2.png";
    std::string rightSrc = "sprites/wizard/wizard2.png";
    std::string leftSrc = "sprites/wizard/wizard2-flip.png";

      player.playerTexture.setFPS(30);
    int cols = 4;
    int cells = 4;
    player.playerTexture.WIMG_Load(src);
    player.setWidth(12);
    player.setHeight(12);
    player.playerTexture.setCells(cells);
    player.playerTexture.setCols(cols);
    player.playerTexture.setFPS(4);

    // Load background texture once
    SDL_Texture* backgroundTex = IMG_LoadTexture(wRenderer, "bg.png");
    if (!backgroundTex) {
        SDL_Log("Failed to load background texture: %s", SDL_GetError());
        tmx_map_free(map);
        return false;
    }
    int bgTexW, bgTexH;
    SDL_QueryTexture(backgroundTex, NULL, NULL, &bgTexW, &bgTexH);

    Timer frameTimer;
    frameTimer.start();
    Uint32 lastTicks = SDL_GetTicks();

    while (!quit) {
   
        Uint32 currentTicks = SDL_GetTicks();
        float deltaTime = (currentTicks - lastTicks) / 1000.0f; // seconds elapsed since last frame
        lastTicks = currentTicks;
	attack = false;
        // Jump logic using jumpIndex
        if (jumpIndex > 0) {
            jump = true;
            jumpIndex = (jumpIndex + 1) % 5 * deltaTime * 5;
        } else {
            jumpIndex = 0;
            jump = false;
        }

        // Input handling
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                quit = true;
            if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    moveLeft = true;
                    player.playerTexture.WIMG_Load(leftSrc);
                    break;
                case SDLK_RIGHT:
                    moveRight = true;
                    player.playerTexture.WIMG_Load(rightSrc);
                    break;
                case SDLK_SPACE:
                    if (jumpIndex < 10) {
                        jumpIndex = 1;
                        jump = true;
                    }
                    break;
		case SDLK_1:
                    attack = true;
		    break;
                }
            }
            if (e.type == SDL_KEYUP && e.key.repeat == 0) {
                switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    moveLeft = false;
                    break;
                case SDLK_RIGHT:
                    moveRight = false;
                    break;
                default:
                    player.playerTexture.WIMG_Load(src);
                    break;
                }
            }
        }
        // Update camera position
        int map_width_px = map->width * map->tile_width;
        int map_height_px = map->height * map->tile_height;
        float scale = 4.0f;
        int scaledScreenWidth = SCREEN_WIDTH / scale;
        int scaledScreenHeight = SCREEN_HEIGHT / scale;
        if (!jump)
            camera.y = player.kyPos + player.height() / 2 - scaledScreenHeight / 2;
        camera.x = player.kxPos + player.width() / 2 - scaledScreenWidth / 2;

        // Clamp camera inside map bounds
        if (camera.x < 0) camera.x = 0;
        if (camera.y < 0) camera.y = 0;
        if (camera.x > map_width_px - camera.w) camera.x = map_width_px - camera.w;
        if (camera.y > map_height_px - camera.h) camera.y = map_height_px - camera.h;
        camera.w = scaledScreenWidth;
        camera.h = scaledScreenHeight;

        // Updates

 
// Update enemies and check collisions
for (auto& enemy : enemies) {
    enemy.enemyUpdate(deltaTime, map);
    SDL_Rect updateRect = {enemy.rect.x - camera.x, enemy.rect.y - camera.y, enemy.rect.w, enemy.rect.h};
    for (auto& pro : projectiles) {
        if (!pro) continue; 
	if (checkCollisionB(updateRect , pro->proRect)) {
	    enemy.health -= pro->power;
	    enemy.jumpFrames = 2;
            pro->destroyMe = true;	
        }

    }
}
enemies.erase(
    std::remove_if(enemies.begin(), enemies.end(),
                   [](const moving_tile& e){ return e.health <= 0; }),
    enemies.end()
);
// Remove projectiles marked for destruction safely
        // Update player logic
        player.update(deltaTime);
        
// --- Parallax background rendering ---

    // Parallax factor: smaller = slower movement
    float parallaxFactorX = 0.4f;
    float parallaxFactorY = 0.3f;

    // How much of the background we want to show on screen
    // Keep it equal to screen size to avoid stretching
    SDL_Rect bgSrcRect;
    bgSrcRect.w = SCREEN_WIDTH;
    bgSrcRect.h = SCREEN_HEIGHT;

    // Offset in background based on camera position
    bgSrcRect.x = (int)(camera.x * parallaxFactorX) % bgTexW;
    bgSrcRect.y = (int)(camera.y * parallaxFactorY) % bgTexH;

    if (bgSrcRect.x < 0) bgSrcRect.x += bgTexW;
    if (bgSrcRect.y < 0) bgSrcRect.y += bgTexH;

    SDL_Rect bgDstRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

    // Render background — handle wrap-around for X and Y
    // Case 1: fits without wrap
    if (bgSrcRect.x + bgSrcRect.w <= bgTexW && bgSrcRect.y + bgSrcRect.h <= bgTexH) {
        SDL_RenderCopy(wRenderer, backgroundTex, &bgSrcRect, &bgDstRect);
    }
    // Case 2: Wrap horizontally or vertically
    else {
        // First chunk
        SDL_Rect src1 = bgSrcRect;
        SDL_Rect dst1 = bgDstRect;

        // Adjust widths/heights for wrap
        if (src1.x + src1.w > bgTexW) src1.w = bgTexW - src1.x;
        if (src1.y + src1.h > bgTexH) src1.h = bgTexH - src1.y;

        SDL_RenderCopy(wRenderer, backgroundTex, &src1, &dst1);

        // Second chunk (horizontal wrap)
        if (bgSrcRect.x + bgSrcRect.w > bgTexW) {
            SDL_Rect src2 = { 0, bgSrcRect.y, bgSrcRect.w - src1.w, src1.h };
            SDL_Rect dst2 = { src1.w, 0, dst1.w - src1.w, dst1.h };
            SDL_RenderCopy(wRenderer, backgroundTex, &src2, &dst2);
        }

        // Second chunk (vertical wrap)
        if (bgSrcRect.y + bgSrcRect.h > bgTexH) {
            SDL_Rect src3 = { bgSrcRect.x, 0, src1.w, bgSrcRect.h - src1.h };
            SDL_Rect dst3 = { 0, src1.h, dst1.w, dst1.h - src1.h };
            SDL_RenderCopy(wRenderer, backgroundTex, &src3, &dst3);
        }
    }

	// Render map and actors
        render_map(map);
        player.moveRender(moveRight, moveLeft, jump, attack, projectiles);
	for(auto enemy: enemies)
	    draw_moving_tile(map, &enemy);
	playerX = player.kxPos;
	playerY = player.kyPos;
        SDL_RenderPresent(wRenderer);

        // Cap frame rate (~60 FPS)
        Uint32 frameTicks = SDL_GetTicks() - currentTicks;
        if (frameTicks < 16)
            SDL_Delay(16 - frameTicks);
    }

    // Cleanup
    SDL_DestroyTexture(backgroundTex);
    tmx_map_free(map);
    return true;
}

