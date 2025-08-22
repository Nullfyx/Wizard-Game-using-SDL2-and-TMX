
#include "render_loop.hpp"
#include "bounding_box.hpp"
#include "camera.hpp"
#include "globals.hpp"
#include "map.hpp"
#include "mapglobal.hpp"
#include "projectile.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <vector>
vector<projectile *> projectiles = {};
bool renderLoop(const char *path) {
  bool isHit = false;
  float hitTimer = 0.0f;

  bool incDt = false;
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
  SDL_Texture *backgroundTex = IMG_LoadTexture(wRenderer, "bg1.png");
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
    float deltaTime = (currentTicks - lastTicks) /
                      1000.0f; // seconds elapsed since last frame
    lastTicks = currentTicks;

    if (jump) {
    player.jumpTimer += deltaTime;
      if (player.jumpTimer >= 0.5f) { // half-second jump
        jump = false;
        player.jumpTimer = 0.0f;
      }
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
          jump = true;

          break;
        case SDLK_1:
          incDt = true;
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
          incDt = false;
        default:
          player.playerTexture.WIMG_Load(src);
          break;
        }
      }
    }
    if(jump && moveLeft)
    {
      player.playerTexture.WIMG_Load(leftSrc);
    }
    if (incDt) {
      player.accDt += deltaTime;
    }
    if (player.accDt >= player.cooldown) {
      attack = true;
      player.accDt = 0;
    } else
      attack = false;
    // Update camera position
    int map_width_px = map->width * map->tile_width;
    int map_height_px = map->height * map->tile_height;
    float scale = 4.0f;
    int scaledScreenWidth = SCREEN_WIDTH / scale;
    int scaledScreenHeight = SCREEN_HEIGHT / scale;
    Vec2 playerPos = {(float)player.xPos() + player.width() / 2.0f,
                      (float)player.yPos() + player.height() / 2.0f};
    camera.Update(playerPos, 30.0f, 0.0f, 5.0f, deltaTime);
    // Clamp camera inside map bounds
    if (camera.rect.x < 0)
      camera.rect.x = 0;
    if (camera.rect.y < 0)
      camera.rect.y = 0;
    if (camera.rect.x > map_width_px - camera.rect.w)
      camera.rect.x = map_width_px - camera.rect.w;
    if (camera.rect.y > map_height_px - camera.rect.h)
      camera.rect.y = map_height_px - camera.rect.h;
    camera.rect.w = scaledScreenWidth;
    camera.rect.h = scaledScreenHeight;

    // Updates

    // Update enemies and check collisions

    for (auto &enemy : enemies) {
      enemy.enemyUpdate(deltaTime, map);
      SDL_Rect dst{enemy.rect.x - camera.rect.x, enemy.rect.y - camera.rect.y,
                   enemy.rect.w, enemy.rect.h};
      enemy.texture.animateSprite(wRenderer, enemy.texture.getCols(),
                                  enemy.texture.getCells(), &dst,
                                  enemy.texture.angle);

      // enemy death fade out...
      Uint8 a;
      enemy.texture.readAlpha(a);
      if (enemy.health <= 0) {
        if (enemy.texture.angle < 90) {
          enemy.texture.angle += 5;
        }
        if ((float)a > 0) {
          a -= 5;
          if ((float)a < 0)
            a = 0;
        }
        enemy.texture.setAlpha(a);
      }

      // projectile collisions
      for (auto &pro : projectiles) {
        if (!pro)
          continue;
        if (checkCollisionB(dst, pro->proRect)) {
          enemy.health -= pro->power;
          if (enemy.health < 0)
            enemy.health = 0;
          if (enemy.health > 10)
            enemy.health = 10;
          pro->destroyMe = true;
        }
      }

      if (checkCollisionB(enemy.rect, *(player.getCollider()))) {
        enemy.accDt += deltaTime;

        if (enemy.accDt >= enemy.cooldown) {
          enemy.accDt = 0.0f; // reset for *next* attack

          int newLives = player.lives() - enemy.atk;
          if (newLives < 0)
            newLives = 0;
          player.setLives(newLives);

          if (newLives > 0) {
            isHit = true;
            hitTimer = 0.2f;
            player.playerTexture.setColor(255, 0, 0);
          }
        }
      } else {
        enemy.accDt = enemy.cooldown;
        // so next time player collides, attack triggers quickly
      }
    }
    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
                                 [](moving_tile &e) {
                                   Uint8 a;
                                   e.texture.readAlpha(a);
                                   return (e.health <= 0 && a <= 0);
                                 }),
                  enemies.end());
    cout << player.lives() << endl;
    // Remove projectiles marked for destruction safely
    // Update player logic
    player.update(deltaTime);

    // --- Parallax background rendering (tiled) ---
    float parallaxFactorX = 0.4f;
    float parallaxFactorY = 0.3f;

    // Offset background movement with camera
    int offsetX = (int)(camera.rect.x * parallaxFactorX);
    int offsetY = (int)(camera.rect.y * parallaxFactorY);

    // Make the texture appear "smaller" by scaling down draw size
    // Example: draw at half size -> background looks zoomed OUT
    int scaleDown = 2; // try 2, 3, or 4 for more repeats

    int tileW = bgTexW / scaleDown;
    int tileH = bgTexH / scaleDown;

    // Loop through screen and tile the background
    for (int y = -offsetY % tileH; y < SCREEN_HEIGHT; y += tileH) {
      for (int x = -offsetX % tileW; x < SCREEN_WIDTH; x += tileW) {
        SDL_Rect dst = {x, y, tileW, tileH};
        SDL_RenderCopy(wRenderer, backgroundTex, NULL, &dst);
      }
    }
    if (isHit) {
      hitTimer -= deltaTime;
      if (hitTimer <= 0.0f) {
        isHit = false;
        player.playerTexture.setColor(255, 255, 255); // back to normal
      }
    }

    // Render map and actors
    render_map(map);
    if (!isDead)
      player.moveRender(moveRight, moveLeft, jump, attack, projectiles);
    isDead = false;
    if (player.lives() <= 0) {
      shouldRestart = true;
      break;
    }
    if (player.yPos() > map->height * map->tile_height) {
      shouldRestart = true;
      break;
    }
    for (auto e : enemies) {
      SDL_Rect dst{e.rect.x - camera.rect.x, e.rect.y - camera.rect.y, e.rect.w,
                   e.rect.h};
      e.texture.animateSprite(wRenderer, e.texture.getCols(),
                              e.texture.getCells(), &dst, e.texture.angle);
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
