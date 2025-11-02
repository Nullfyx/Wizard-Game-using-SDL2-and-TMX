#include "bounding_box.hpp"
#include "camera.hpp"
#include "globals.hpp"
#include "lightSystem.hpp"
#include "map.hpp"
#include "mapglobal.hpp"
#include "particleSystem.hpp"
#include "projectile.hpp"
#include "render_loop.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
vector<projectile *> projectiles = {};

// --- FIX ---
// Update function definition to match header
bool renderLoop(const char *path, TTF_Font *globalFont,
                SDL_Texture *backgroundTex) {
  // --- END FIX ---
  bool isHit = false;
  float hitTimer = 0.0f;
  LightSystem l(1, 0.5f);
  ParticleSystem particles(wRenderer, 4.0f);
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
  bool isDown = false;
  // Player setup
  Player player;
  player.setMap(map);

  WTexture heroRightTex;
  heroRightTex.WIMG_Load("sprites/hero-flipped.png");

  WTexture heroLeftTex;
  heroLeftTex.WIMG_Load("sprites/hero.png");
  int cols = 6;
  int cells = 6;
  heroRightTex.setCols(cols);
  heroRightTex.setCells(cells);
  heroLeftTex.setCols(cols);
  heroLeftTex.setCells(cells);

  player.playerTexture.setFPS(4);

  player.playerTexture = heroRightTex;

  player.setWidth(16);
  player.setHeight(16);

  // --- We no longer need to set this on the player directly ---
  // player.playerTexture.setCells(cells);
  // player.playerTexture.setCols(cols);
  // ---

  player.draw = false;

  // --- FIX ---
  // Use the font that was passed in, don't load a new one.
  font = globalFont;
  if (!font)
    cout << "font is null! " << TTF_GetError() << endl;
  // --- END FIX ---

  // --- FIX ---
  // Use the background texture that was passed in.
  // We don't need to load it.
  if (!backgroundTex) {
    SDL_Log("Background texture was null!");
    tmx_map_free(map);
    return false;
  }
  // --- END FIX ---

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
    if (LightSystem::active) {
      LightSystem::active->clearLights();
    }

    if (jump) {
      player.jumpTimer += deltaTime;
      if (player.jumpTimer >= 0.5f) { // half-second jump
        jump = false;
        player.jumpTimer = 0.0f;
      }
    }
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT)
        quit = true;
      player.handleMenuEvent(e);
      // Input handling

      if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
        case SDLK_LEFT:
          moveLeft = true;
          // --- FIX ---
          // Don't load from disk! Just assign the loaded texture.
          // This now works because heroLeftTex has correct cols/cells.
          player.playerTexture = heroLeftTex;
          // --- END FIX ---
          break;
        case SDLK_RIGHT:
          moveRight = true;
          // --- FIX ---
          // Don't load from disk! Just assign the loaded texture.
          // This now works because heroRightTex has correct cols/cells.
          player.playerTexture = heroRightTex;
          // --- END FIX ---
          break;
        case SDLK_SPACE:
          jump = true;

          break;
        case SDLK_1:
          incDt = true;
          break;
        case SDLK_DOWN:
          isDown = true;
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
          break;
        case SDLK_DOWN:
          isDown = false;
          break;
        default:
          // --- FIX ---
          // If no keys are pressed, set to a default (e.g., right-facing)
          if (!moveLeft && !moveRight) {
            player.playerTexture = heroRightTex;
          }
          // --- END FIX ---
          break;
        }
      }
    }
    if (jump && moveLeft) {
      // --- FIX ---
      player.playerTexture = heroLeftTex;
      // --- END FIX ---
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

    camera.Update(playerPos, 30.0f, 5.0f, 5.0f, deltaTime);

    // Clamp camera inside map bounds

    Vec2 currentPos = camera.GetFloatPosition();

    // Update dimensions (assuming needed here)
    camera.rect.w = scaledScreenWidth;
    camera.rect.h = scaledScreenHeight;

    // Clamp Left/Right
    if (currentPos.x < 0.0f) {
      currentPos.x = 0.0f;
    } else if (currentPos.x > map_width_px - camera.rect.w) {
      currentPos.x = map_width_px - camera.rect.w;
    }

    // Clamp Top/Bottom
    if (currentPos.y < 0.0f) {
      currentPos.y = 0.0f;
    } else if (currentPos.y > map_height_px - camera.rect.h) {
      currentPos.y = map_height_px - camera.rect.h;
    }

    // Apply clamped position
    camera.SetFloatPosition(currentPos);
    // Updates

    // Update enemies and check collisions

    for (auto &enemy : enemies) {
      enemy.enemyUpdate(deltaTime, map);
      float dx = (float)enemy.rect.x - camera.GetFloatPosition().x;
      float dy = (float)enemy.rect.y - camera.GetFloatPosition().y;

      SDL_Rect dst{
          // Position: (World Difference * Zoom) -> Round -> Cast to Int
          (int)std::round(dx), (int)std::round(dy),

          // Dimensions: (World Size * Zoom) -> Round -> Cast to Int
          (int)std::round((float)enemy.rect.w),
          (int)std::round((float)enemy.rect.h)};
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
          if ((float)a <= 0) {
            a = 0;
            *mana += enemy.mana;
          }
        }
        enemy.texture.setAlpha(a);
        ParticleSystem::active->setSpeedRange(0.2f, 1.5f);
        ParticleSystem::active->setLifeRange(0.1f, 0.3f);
        ParticleSystem::active->setSizeRange(0.2f, 1.3f);
        ParticleSystem::active->setBaseColor(255, 25, 25, 100);
        ParticleSystem::active->setColorVariance(50);
        float centerX = enemy.rect.x + enemy.rect.w / 2.0f;
        float centerY = enemy.rect.y + enemy.rect.h / 2.0f;
        ParticleSystem::active->emit(centerX, centerY, 3);
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
    // Remove projectiles marked for destruction safely
    // Update player logic
    player.update(deltaTime);

    // --- Parallax background rendering (tiled) ---
    float parallaxFactorX = 0.4f;
    float parallaxFactorY = 0.3f;

    // Offset background movement with camera
    float offsetX = (camera.GetFloatPosition().x * parallaxFactorX);
    float offsetY = (camera.GetFloatPosition().y * parallaxFactorY);
    // printf("%f", camera.GetFloatPosition().x);

    // Make the texture appear "smaller" by scaling down draw size
    // Example: draw at half size -> background looks zoomed OUT
    int scaleDown = 3; // try 2, 3, or 4 for more repeats

    float tileW = bgTexW / scaleDown;
    float tileH = bgTexH / scaleDown;
    // Loop through the screen and tile the background properly
    for (float y = fmodf(-offsetY, tileH); y < SCREEN_HEIGHT; y += tileH) {
      for (float x = fmodf(-offsetX, tileW); x < SCREEN_WIDTH; x += tileW) {
        SDL_FRect dst = {x, y, (float)tileW, (float)tileH};
        SDL_RenderCopyF(wRenderer, backgroundTex, NULL, &dst);
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
    render_map(map, deltaTime);
    if (!isDead)
      player.moveRender(moveRight, moveLeft, jump, attack, projectiles, isDown);
    isDead = false;
    if (player.lives() <= 0 || player.yPos() > map_height_px) {
      float centerX = player.kxPos + player.width() / 2.0f;
      float centerY = player.kyPos + player.height() / 2.0f;

      if (ParticleSystem::active) {
        ParticleSystem::active->setSpeedRange(0.5f, 5.0f);
        ParticleSystem::active->setLifeRange(0.5f, 1.0f);
        ParticleSystem::active->setSizeRange(1.0f, 6.0f);
        ParticleSystem::active->setBaseColor(255, 255, 255, 255);
        ParticleSystem::active->setColorVariance(50);
        ParticleSystem::active->emit(centerX, centerY, 100);
        ParticleSystem::active->setGravity(true);
      }

      // Animate particles for ~0.5 seconds
      Uint32 start = SDL_GetTicks();
      while (SDL_GetTicks() - start < 500) { // 500ms pause
        float dt = 0.016f;                   // roughly 60 FPS
        ParticleSystem::active->update(dt);
        ParticleSystem::active->render(wRenderer, 3.0f,
                                       camera.GetFloatPosition().x,
                                       camera.GetFloatPosition().y);
        SDL_RenderPresent(wRenderer);
        SDL_Delay(16); // cap ~60 FPS
      }

      isDead = true;
      shouldRestart = true;
      break;
    }

    for (auto p : projectiles) {
      if (p->proRect.x + p->proRect.w < 0 || p->proRect.x > SCREEN_WIDTH ||
          p->proRect.y + p->proRect.h < 0 || p->proRect.y > SCREEN_HEIGHT) {
        p->destroyMe = true;
      }
    }
    for (auto e : enemies) {

      // 1. Calculate the smooth world difference (float)
      float dx = (float)e.rect.x - camera.GetFloatPosition().x;
      float dy = (float)e.rect.y - camera.GetFloatPosition().y;

      // 2. Create the SDL_Rect, applying ROUNDING to the world difference
      //    but NOT applying the zoom factor. We use the original e.rect.w/h.
      SDL_Rect dst = {// Position: (World Difference) -> Round -> Cast to Int
                      (int)std::round(dx), (int)std::round(dy),

                      // Dimensions: Use original, unscaled world units
                      e.rect.w, e.rect.h};

      // 3. Let the animateSprite function apply the scaling as it did before.
      e.texture.animateSprite(wRenderer, e.texture.getCols(),
                              e.texture.getCells(), &dst, e.texture.angle);
    }
    for (auto e : enemies) {

      // 1. Calculate the smooth world difference (float)
      float dx = (float)e.rect.x - camera.GetFloatPosition().x;
      float dy = (float)e.rect.y - camera.GetFloatPosition().y;

      // 2. Create the SDL_Rect, applying ROUNDING to the world difference
      //    but NOT applying the zoom factor. We use the original e.rect.w/h.
      SDL_Rect dst = {// Position: (World Difference) -> Round -> Cast to Int
                      (int)std::round(dx), (int)std::round(dy),

                      // Dimensions: Use original, unscaled world units
                      e.rect.w, e.rect.h};

      // 3. Let the animateSprite function apply the scaling as it did before.
      e.texture.animateSprite(wRenderer, e.texture.getCols(),
                              e.texture.getCells(), &dst, e.texture.angle);
    }
    playerX = player.kxPos;
    playerY = player.kyPos;
    ParticleSystem::active->update(deltaTime);
    LightSystem::active->update(deltaTime);

    ParticleSystem::active->render(wRenderer, 3.0, camera.GetFloatPosition().x,
                                   camera.GetFloatPosition().y);
    if (LightSystem::active) {
      LightSystem::active->render(wRenderer);
    }
    player.lifeUpdate();
    SDL_RenderPresent(wRenderer);

    // Cap frame rate (~60 FPS)
    Uint32 frameTicks = SDL_GetTicks() - currentTicks;
    if (frameTicks < 16)
      SDL_Delay(16 - frameTicks);
  }

  // Cleanup
  // --- FIX ---
  // We no longer destroy the background texture here,
  // it will be destroyed in main.cpp
  // --- END FIX ---
  tmx_map_free(map);
  return true;
}
