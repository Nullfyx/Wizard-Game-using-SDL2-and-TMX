#include "player.hpp"
#include <SDL2/SDL_render.h>
#include "projectile.hpp"
Player::Player()
{
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
};

// getters and setters (all)
int Player::width()
{
    return wWidth;
}

int Player::height()
{
    return wHeight;
}

double Player::weight()
{
    return wWeight;
}

double Player::maxVel()
{
    return wMaxVel;
}

double Player::atk()
{
    return wAtk;
}

int Player::health()
{
    return wHealth;
}

int Player::lives()
{
    return wLives;
}

int Player::xPos()
{
    return wXPos;
}

int Player::yPos()
{
    return wYPos;
}

int Player::xVel()
{
    return wXVel;
}

int Player::yVel()
{
    return wYVel;
}

double Player::gravityVel()
{
    return wGravityVel;
}

SDL_Rect *Player::getCollider()
{
    return &playerRect;
}

auto Player::getMap()
{
    return map;
}

bool Player::getisGrounded()
{
    return isGrounded;
}

double Player::jumpPower()
{
    return wJumpPower;
}

bool Player::isJumping()
{
    return jumping;
}

void Player::setWidth(int w)
{
    wWidth = w;
}

void Player::setHeight(int h)
{
    wHeight = h;
}

void Player::setWeight(double w)
{
    wWeight = w;
}

void Player::setMaxVel(double m)
{
    wMaxVel = m;
}

void Player::setAtk(double a)
{
    wAtk = a;
}

void Player::setHealth(int h)
{
    wHealth = h;
}

void Player::setLives(int l)
{
    wLives = l;
}

void Player::setXPos(int x)
{
    wXPos = x;
}

void Player::setYPos(int y)
{
    wYPos = y;
}

void Player::setXVel(int x)
{
    wXVel = x;
}

void Player::setYVel(int y)
{
    wYVel = y;
}

void Player::setCollider(SDL_Rect p)
{
    playerRect = p;
}

void Player::setFlip(SDL_RendererFlip f)
{
    flip = f;
}

void Player::setRotate(double a)
{
    rotate = a;
}

void Player::setGravityVel(double g)
{
    wGravityVel = g;
}

void Player::setJumpPower(double p)
{
    wJumpPower = p;
}

void Player::setMap(tmx_map *m)
{
    std::cout << "[DEBUG] Map set to player: " << m << std::endl;

    map = m;
}

void Player::setisGrounded(bool u)
{
    isGrounded = u;
}

void Player::setJumping(bool j)
{
    jumping = j;
}
void Player::moveRender(bool moveRight, bool moveLeft, bool jump, bool attack, vector<projectile*> &projectiles)
{

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
    bool onGround = false, wallLeft = false, wallRight = false, onCeiling = false, overlapping = false;
    checkCollisionsXY(map, onGround, wallLeft, wallRight, onCeiling, overlapping, playerRect);
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
    if (jump && onGround) {
        applyForce(0, -6300); // instant upward push
    }
if (attack)
{
    // Pick nearest enemy at spawn
    float closestDist = FLT_MAX;
    float targetX = kxPos; // default to player if no enemies
    float targetY = kyPos;

    for (const auto &enemy : enemies) {
        float dx = enemy.x - kxPos;
        float dy = enemy.y - kyPos;
        float distSq = dx*dx + dy*dy;
        if (distSq < closestDist) {
            closestDist = distSq;
            targetX = enemy.x;
            targetY = enemy.y;
        }
    }
    // Create projectile with exact physics positions
    projectile *p = new projectile(static_cast<int>(kxPos), static_cast<int>(kyPos),
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
        kvelocityX     = kvelocityX > 0 ? 0 : kvelocityX;
        kaccelerationX = kaccelerationX > 0 ? 0 : kaccelerationX;
        kforceX        = kforceX > 0 ? 0 : kforceX;
        passThisFramePosX = true;
    }
    if (wallLeft) {
        kvelocityX     = kvelocityX < 0 ? 0 : kvelocityX;
        kaccelerationX = kaccelerationX < 0 ? 0 : kaccelerationX;
        kforceX        = kforceX < 0 ? 0 : kforceX;
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
    for (int i = projectiles.size() - 1; i >= 0; --i) {
    if (projectiles[i]->destroyMe) {
        delete projectiles[i];
        projectiles.erase(projectiles.begin() + i);
    } else {
        projectiles[i]->update();
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
    SDL_Rect screenRect = { 
        static_cast<int>(playerRect.x - camera.rect.x),
        static_cast<int>(playerRect.y - camera.rect.y),
        playerRect.w,
        playerRect.h
    };
    
    playerTexture.animateSprite(wRenderer, playerTexture.getCols(), playerTexture.getCells(), screenRect, rotate, NULL, flip);
}

void Player::update(float d)
{
    // cout << isGrounded << endl;
    dt = d;
}

void Player::jump()
{
    if (!jumping)
    {
        wYVel += wJumpPower;
        jumping = true;
    }
}
