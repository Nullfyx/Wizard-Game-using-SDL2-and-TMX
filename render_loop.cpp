
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
bool renderLoop(const char *path)
{
    int jumpIndex = 0;
    SDL_Event e;
    bool quit = false;
    bool isDead = false;
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
		case SDLK_1:
		    attack = false;
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
    SDL_Rect dst{
    enemy.rect.x - camera.x,
    enemy.rect.y - camera.y,
    enemy.rect.w,
    enemy.rect.h
};
enemy.texture.animateSprite(wRenderer, enemy.texture.getCols(), enemy.texture.getCells(), dst, enemy.texture.angle)
;
        // Set alpha b`ased on health
	    		   Uint8 a;
                           enemy.texture.readAlpha(a);
if (enemy.health <= 0) {
	     if(enemy.texture.angle < 90)
    {
	enemy.texture.angle += 5;
    }

    // Reduce alpha gradually
    if ((float)a > 0) {
        a -= 5; 
	if ((float)a < 0) a = 0; 
    }
       enemy.texture.setAlpha(a);
}    for (auto& pro : projectiles) {
        if (!pro) continue; 

        if (checkCollisionB(dst , pro->proRect) && pro->canFire()) {
            enemy.health -= pro->power;
            cout  << pro->canFire() << endl;
            // Clamp health to valid range
            if (enemy.health < 0) enemy.health = 0;
            if (enemy.health > 10) enemy.health = 10;

	    enemy.texture.readAlpha(a);
		cout << (float)a << endl;
            pro->destroyMe = true;	
        }
    }

if(checkCollisionB(enemy.rect, *(player.getCollider())))
{
   if(!isDead)
       player.setLives(std::max(0, player.lives() - enemy.atk));
}

}
enemies.erase(
    std::remove_if(enemies.begin(), enemies.end(),
                   [](moving_tile& e){ 	  
		   Uint8 a;
	    e.texture.readAlpha(a);
return (e.health <= 0 && a <= 0); }),
    enemies.end()
);
cout << player.lives() <<endl;
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
        if(!isDead)player.moveRender(moveRight, moveLeft, jump, attack, projectiles);
	isDead = false;
	if(player.lives() <= 0)
{
        shouldRestart = true;
	break;
}

	for(auto e: enemies){
		SDL_Rect dst{
    e.rect.x - camera.x,
    e.rect.y - camera.y,
    e.rect.w,
    e.rect.h
};
	    e.texture.animateSprite(wRenderer, e.texture.getCols(), e.texture.getCells(), dst, e.texture.angle);
	}
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

