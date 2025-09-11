#include "globals.hpp"
#include "particleSystem.hpp"
#include "player.hpp"
#include "projectile.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

Player::Player() {
  wWidth = 20;
  wHeight = 20;
  wWeight = 2000; // N
  wMaxVel = 1;
  wAtk = 10;  // weak at the begininning!
  wLives = 9; // maybe its a cat!
  wXPos = 0;
  isGrounded = false;
  wYPos = 180;
  wXVel = 0;
  wGravityVel = (wWeight / gravity.g()) * dt * 10;
  wYVel = wGravityVel;
  flip = SDL_FLIP_NONE;
  rotate = 0.00;
  wJumpPower = -100; // Newtons
  map = nullptr;
  jumping = false;
  isRoofed = false;
  isRightObstructed = false;
  isLeftObstructed = false;
  accDt = 0;
  cooldown = 0.5;
  playerRect = {0, 0, 0, 0};
  jumpTimer = 0.0f;
  uiFont = TTF_OpenFont("./PixelOperator.ttf", 16);

  SDL_Surface *s1 = IMG_Load("sprites/ui/clock.png");
  SDL_Surface *s2 = IMG_Load("sprites/ui/greenHeart.png");
  SDL_Surface *s3 = IMG_Load("sprites/ui/magic.png");
  cardTextures = {SDL_CreateTextureFromSurface(wRenderer, s1),
                  SDL_CreateTextureFromSurface(wRenderer, s2),
                  SDL_CreateTextureFromSurface(wRenderer, s3)};
  SDL_FreeSurface(s1);
  SDL_FreeSurface(s2);
  SDL_FreeSurface(s3);
};

// getters and setters (all)
int Player::width() { return wWidth; }

int Player::height() { return wHeight; }

double Player::weight() { return wWeight; }

double Player::maxVel() { return wMaxVel; }

double Player::atk() { return wAtk; }

int Player::health() { return wHealth; }

int Player::lives() { return wLives; }

int Player::xPos() { return wXPos; }

int Player::yPos() { return wYPos; }

int Player::xVel() { return wXVel; }

int Player::yVel() { return wYVel; }

double Player::gravityVel() { return wGravityVel; }

SDL_Rect *Player::getCollider() { return &playerRect; }

auto Player::getMap() { return map; }

bool Player::getisGrounded() { return isGrounded; }

double Player::jumpPower() { return wJumpPower; }

bool Player::isJumping() { return jumping; }

void Player::setWidth(int w) { wWidth = w; }

void Player::setHeight(int h) { wHeight = h; }

void Player::setWeight(double w) { wWeight = w; }

void Player::setMaxVel(double m) { wMaxVel = m; }

void Player::setAtk(double a) { wAtk = a; }

void Player::setHealth(int h) { wHealth = h; }

void Player::setLives(int l) { wLives = l; }

void Player::setXPos(int x) { wXPos = x; }

void Player::setYPos(int y) { wYPos = y; }

void Player::setXVel(int x) { wXVel = x; }

void Player::setYVel(int y) { wYVel = y; }

void Player::setCollider(SDL_Rect p) { playerRect = p; }

void Player::setFlip(SDL_RendererFlip f) { flip = f; }

void Player::setRotate(double a) { rotate = a; }

void Player::setGravityVel(double g) { wGravityVel = g; }

void Player::setJumpPower(double p) { wJumpPower = p; }

void Player::setMap(tmx_map *m) { map = m; }

void Player::setisGrounded(bool u) { isGrounded = u; }

void Player::setJumping(bool j) { jumping = j; }
void Player::print(bool onGround, bool onCeiling, bool wallLeft, bool wallRight,
                   bool overlapping) {
  cout << "xPos: " << kxPos << " " << wXPos << endl;
  cout << "yPos: " << kyPos << " " << wYPos << endl;
  cout << "xVel: " << kvelocityX << endl;
  cout << "yVel: " << kvelocityY << endl;
  cout << "ForceX: " << kforceX << endl;
  cout << "ForceY: " << kforceY << endl;
  cout << "AccX: " << kaccelerationX << endl;
  cout << "AccY: " << kaccelerationY << endl;
  cout << onGround << "onground" << endl;
  cout << wallLeft << "wallleft" << endl;
  cout << wallRight << "wallRight" << endl;
  cout << onCeiling << ":onCeiling" << endl;
  cout << overlapping << ": overlapping" << endl;
}
void Player::moveRender(bool moveRight, bool moveLeft, bool jump, bool attack,
                        vector<projectile *> &projectiles) {

  // ===== RESET FRAME FLAGS =====
  passThisFrameYPos = false;
  passThisFrameNegX = false;
  passThisFrameYNeg = false;
  passThisFramePosX = false;

  jumping = jump;
  kdt = dt;

  // Sync world positions to kinematics
  kxPos = static_cast<double>(wXPos);
  kyPos = static_cast<double>(wYPos);
  playerVelX = kvelocityX;
  playerVelY = kvelocityY;

  // ===== COLLISION DETECTION =====
  bool onGround = false, wallLeft = false, wallRight = false, onCeiling = false,
       overlapping = false;
  checkCollisionsXY(map, onGround, wallLeft, wallRight, onCeiling, overlapping,
                    playerRect);

  kvelocityY = playerVelY;
  kvelocityX = playerVelX;
  // ===== GRAVITY =====
  if (!onGround) {
    applyForce(0, (wWeight * kgravityConstant * dt)); // pull down
  } else {
    if (kvelocityY > 0) { // only stop downward movement
      kvelocityY = 0;
      passThisFrameYPos = true;
    }
  }
  // ===== INPUT FORCES =====
  if (moveLeft) {
    applyForce(-1000, 0);
  }
  if (moveRight) {
    applyForce(1000, 0);
  }
  if (jump && onGround && jumpTimer < 0.1f && !overlapping) {
    applyForce(0, -4900); // instant upward push
  } else if (overlapping) {
    applyForce(0, -5);
  }
  if (attack) {
    // Pick nearest enemy at spawn
    float closestDist = FLT_MAX;
    float targetX = kxPos; // default to player if no enemies
    float targetY = kyPos; // setup

    for (const auto &enemy : enemies) {
      float dx = enemy.x - kxPos;
      float dy = enemy.y - kyPos;
      float distSq = dx * dx + dy * dy;
      if (distSq < closestDist) {
        closestDist = distSq;
        targetX = enemy.x + (enemy.width / 2);
        targetY = enemy.y + (enemy.height / 2);
      }
    }

    // Create projectile with exact physics positions
    projectile *p =
        new projectile(static_cast<int>(kxPos), static_cast<int>(kyPos),
                       static_cast<int>(targetX), static_cast<int>(targetY));
    projectiles.push_back(p);
  }

  // ===== FRICTION =====
  if (onGround && (!(moveLeft || moveRight))) {
    if (std::abs(kvelocityX) > 0.3) {
      kvelocityX *= 0.85;
    } else {
      kvelocityX = 0;
    }
  }

  // ===== PREVENT TUNNELING =====
  if (wallRight) {
    kvelocityX = kvelocityX > 0 ? 0 : kvelocityX;
    kaccelerationX = kaccelerationX > 0 ? 0 : kaccelerationX;
    kforceX = kforceX > 0 ? 0 : kforceX;
    passThisFramePosX = true;
  }
  if (wallLeft) {
    kvelocityX = kvelocityX < 0 ? 0 : kvelocityX;
    kaccelerationX = kaccelerationX < 0 ? 0 : kaccelerationX;
    kforceX = kforceX < 0 ? 0 : kforceX;
    passThisFrameNegX = true;
  }
  if (onCeiling) {
    if (kvelocityY < 0) { // only stop upward motion
      kvelocityY = 0;
      passThisFrameYNeg = true;
    }
  }
  kmaxVel = wMaxVel;
  // ===== PHYSICS MOVE =====
  move();
  // print();
  for (int i = projectiles.size() - 1; i >= 0; --i) {
    if (projectiles[i]->destroyMe) {
      delete projectiles[i];
      projectiles.erase(projectiles.begin() + i);
    } else {
      projectiles[i]->update(dt);
    }
  }

  // ===== UPDATE STATE =====
  isGrounded = onGround;
  isRoofed = onCeiling;

  // ===== UPDATE COLLIDER =====
  playerRect.x = static_cast<int>(kxPos);
  playerRect.y = static_cast<int>(kyPos);
  playerRect.w = wWidth;
  playerRect.h = wHeight;

  // Update integer positions
  wXPos = playerRect.x;
  wYPos = playerRect.y;

  // ===== RENDER =====
  SDL_Rect screenRect = {static_cast<int>(playerRect.x - camera.rect.x),
                         static_cast<int>(playerRect.y - camera.rect.y),
                         playerRect.w, playerRect.h};

  playerTexture.animateSprite(wRenderer, playerTexture.getCols(),
                              playerTexture.getCells(), &screenRect, rotate,
                              NULL, flip);
}

void Player::update(float d) {
  // cout << isGrounded << endl;
  dt = d > 0.18 ? 0.16 : d;
}

void Player::jump() {
  if (!jumping) {
    wYVel += wJumpPower;
    jumping = true;
  }
}
void Player::createManaCircleTexture() {
  const int textureSize = 128;
  const int radius = textureSize / 2 - 2; // margin inside texture

  // Create an SDL surface for drawing
  SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(
      0, textureSize, textureSize, 32, SDL_PIXELFORMAT_RGBA32);
  if (!surface) {
    // handle error
    return;
  }

  // Clear surface with transparent color
  SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));

  SDL_LockSurface(surface);
  Uint32 *pixels = (Uint32 *)surface->pixels;

  Uint32 fillColor =
      SDL_MapRGBA(surface->format, 0, 255, 255, 255); // cyan fill
  Uint32 borderColor =
      SDL_MapRGBA(surface->format, 255, 255, 255, 255); // white border

  int cx = textureSize / 2;
  int cy = textureSize / 2;

  // Draw filled circle
  for (int y = 0; y < textureSize; ++y) {
    for (int x = 0; x < textureSize; ++x) {
      int dx = x - cx;
      int dy = y - cy;
      float dist = sqrtf(dx * dx + dy * dy);
      if (dist <= radius) {
        pixels[y * (surface->pitch / 4) + x] = fillColor;
      }
    }
  }
  // Draw circle border
  for (int angle = 0; angle < 360; ++angle) {
    float rad = angle * M_PI / 180.0f;
    int px = (int)(cx + radius * cosf(rad));
    int py = (int)(cy + radius * sinf(rad));
    if (px >= 0 && px < textureSize && py >= 0 && py < textureSize) {
      pixels[py * (surface->pitch / 4) + px] = borderColor;
    }
  }

  SDL_UnlockSurface(surface);

  // Create texture from surface
  if (manaCircleTexture) {
    SDL_DestroyTexture(manaCircleTexture);
    manaCircleTexture = nullptr;
  }
  manaCircleTexture = SDL_CreateTextureFromSurface(wRenderer, surface);
  SDL_SetTextureBlendMode(manaCircleTexture, SDL_BLENDMODE_BLEND);

  SDL_FreeSurface(surface);
}

void Player::lifeUpdate() {
  int totalLives = 9;
  static float displayedLives = (float)wLives;

  // Animate lives
  if (displayedLives > wLives) {
    displayedLives -= 0.05f;
    if (displayedLives < wLives)
      displayedLives = (float)wLives;
  } else if (displayedLives < wLives) {
    displayedLives += 0.05f;
    if (displayedLives > wLives)
      displayedLives = (float)wLives;
  }

  float uiScale = 0.25f;
  int margin = (int)(16 * uiScale);
  int barWidth = (int)(SCREEN_WIDTH * 0.85f * uiScale);
  int barHeight = (int)(20 * uiScale);
  int x = margin, y = margin;

  // Life Bar
  SDL_Rect lifeBg = {x, y, barWidth, barHeight};
  SDL_SetRenderDrawColor(wRenderer, 60, 60, 60, 255);
  SDL_RenderFillRect(wRenderer, &lifeBg);

  float healthPercent = displayedLives / totalLives;
  SDL_Rect lifeFill = {x, y, (int)(barWidth * healthPercent), barHeight};
  SDL_SetRenderDrawColor(wRenderer, 0, 200, 0, 255);
  SDL_RenderFillRect(wRenderer, &lifeFill);

  // Mana logic
  auto getNextThreshold = [](int lvl) {
    int threshold = 0;
    for (int i = 1; i <= lvl; ++i)
      threshold += 8 + (i - 1) * 2;
    return threshold;
  };
  int nextThreshold = getNextThreshold(*level);
  if (*mana >= nextThreshold) {
    *mana -= nextThreshold;
    (*level)++;
    if (*level > 1) {
      draw = true;
    }
    nextThreshold = getNextThreshold(*level);
  }
  float manaPercent = (float)*mana / nextThreshold;

  // Draw high-res mana circle texture scaled down to 20 pixels diameter
  int circleDiameter = 20;
  int circleRadius = circleDiameter / 2;
  int circleX = (int)(SCREEN_WIDTH * uiScale) - circleRadius - margin;
  int circleY = (int)(((y + barHeight) / 2) * uiScale) + circleRadius + margin;

  SDL_Rect dstRect = {circleX - circleRadius, circleY - circleRadius,
                      circleDiameter, circleDiameter};

  SDL_RenderCopy(wRenderer, manaCircleTexture, NULL, &dstRect);

  // Draw filled arc for mana portion (overlay lines)
  int segments = 60;
  SDL_SetRenderDrawColor(wRenderer, 0, 255, 255, 255);
  int endSegment = (int)(segments * manaPercent);
  for (int i = 0; i < endSegment; ++i) {
    float angle = 2.0f * M_PI * i / segments;
    int px = (int)(circleX + circleRadius * cosf(angle));
    int py = (int)(circleY + circleRadius * sinf(angle));
    SDL_RenderDrawLine(wRenderer, circleX, circleY, px, py);
  }
  if (draw) {
    drawMenu();
  }
  cout << draw << endl;
  // Level text
  SDL_Color white = {255, 255, 255, 255};
  std::string levelText = std::to_string(*level);
  SDL_Surface *textSurface =
      TTF_RenderText_Blended(font, levelText.c_str(), white);
  SDL_Texture *textTex = SDL_CreateTextureFromSurface(wRenderer, textSurface);
  int textW, textH;
  SDL_QueryTexture(textTex, NULL, NULL, &textW, &textH);
  SDL_Rect dst = {circleX - textW / 2, circleY - textH / 2, textW, textH};
  SDL_RenderCopy(wRenderer, textTex, NULL, &dst);
  SDL_FreeSurface(textSurface);
  SDL_DestroyTexture(textTex);
}

void Player::drawMenu() {
  if (!draw)
    return; // only draw if active

  SDL_Rect drawRect = {8, 8, (SCREEN_WIDTH - 64) / 4, (SCREEN_HEIGHT - 64) / 4};
  SDL_SetRenderDrawColor(wRenderer, 50, 50, 50, 220);
  SDL_RenderFillRect(wRenderer, &drawRect);

  int cardCount = (int)cardTextures.size();
  int cardWidth = drawRect.w / cardCount;
  int cardHeight = drawRect.h;
  string names[]{"0.8x cooldown", "+2 health", "New magic"};
  for (int i = 0; i < cardCount; i++) {
    int x = 8 + i * cardWidth;
    int y = 8;

    // Highlight border if selected
    SDL_Rect cardRect = {x, y, cardWidth, cardHeight};
    if (i == selectedCard)
      SDL_SetRenderDrawColor(wRenderer, 255, 255, 0, 255); // yellow border
    else
      SDL_SetRenderDrawColor(wRenderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(wRenderer, &cardRect);

    // Keep image square
    int imgSize = std::min(cardWidth - 20, cardHeight - 40);
    SDL_Rect imgRect = {x + (cardWidth - imgSize) / 2, y + 10, imgSize,
                        imgSize};
    SDL_RenderCopy(wRenderer, cardTextures[i], NULL, &imgRect);

    // Draw text
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *txtSurf =
        TTF_RenderText_Solid(uiFont, names[i].c_str(), white);
    SDL_Texture *txtTex = SDL_CreateTextureFromSurface(wRenderer, txtSurf);
    SDL_Rect txtRect = {x + (cardWidth - txtSurf->w) / 2, y + imgSize + 15,
                        txtSurf->w, txtSurf->h};
    SDL_RenderCopy(wRenderer, txtTex, NULL, &txtRect);

    SDL_FreeSurface(txtSurf);
    SDL_DestroyTexture(txtTex);
  }
}
void Player::handleMenuEvent(const SDL_Event &e) {
  if (!draw)
    return; // menu not active

  if (e.type == SDL_KEYDOWN) {
    switch (e.key.keysym.sym) {
    case SDLK_SPACE: // cycle through cards
      selectedCard = (selectedCard + 1) % (int)cardTextures.size();
      break;

    case SDLK_RETURN:
      applyCardEffect(selectedCard);
      draw = false;
      break;
    }
  }
}

void Player::applyCardEffect(int cardIndex) {
  switch (cardIndex) {
  case 0:
    cooldown *= 0.8; // reduce cooldown
    break;
  case 1:
    wLives += 2; // add health
    break;
  case 2:
    break;
  }
}
