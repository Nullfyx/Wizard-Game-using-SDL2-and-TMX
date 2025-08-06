#include "player.hpp"
Player::Player()
{
    wWidth = 20;
    wHeight = 20;
    wWeight = 240; // N
    wMaxVel = 3;
    wAtk = 10;  // weak at the begininning!
    wLives = 9; // maybe its a cat!
    wXPos = 0;
    isGrounded = false;
    wYPos = 0;
    wXVel = 0;
    wGravityVel = (wWeight / gravity.g()) * dt * 10;
    wYVel = wGravityVel;
    flip = SDL_FLIP_NONE;
    rotate = 0.00;
    wJumpPower = -100; // N
    map = nullptr;
    jumping = false;
};

// getters and setters
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

auto Player::getCollider()
{
    return playerRect;
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
void Player::moveRender(bool moveRight, bool moveLeft, bool jump)
{
    jumping = jump;
    kdt = dt;
    // Copy current screen position into kpos (double precision)
    kxPos = static_cast<double>(wXPos);
    kyPos = static_cast<double>(wYPos);

    // Input debug
    cout << "k " << kxPos << " " << kyPos << endl;
    cout << "p " << wXPos << " " << wYPos << endl;
    cout << "isGrounded (before move): " << isGrounded << endl;
    cout << "moveleft: " << moveLeft << ", moveRight: " << moveRight << ", jump: " << jump << endl;

    checkCollisionsXY(map, kxPos, kyPos);

    // Movement inputs
    if (moveLeft && !moveRight)
        applyForce(-1000, 0);
    else if (moveRight && !moveLeft)
        applyForce(1000, 0);
    if (isGrounded)
    {
        if (abs(kvelocityX) > 0.1f)
            kvelocityX *= 0.9f;
        else
            kvelocityX = 0;
    }
    // Jumping
    if (jump && isGrounded)
        applyForce(0, -2000);

    // Apply gravity if airborne
    if (!isGrounded)
        applyForce(0, kmass * kgravityConstant);

    // Apply forces to update kPos
    move();

    // After moving, update tile collision & grounding
    bool touchingGround = isTileCollidableY();
    bool touchingWall = isTileCollidableX();

    // If jumping and hit a ceiling
    if (jumping && isTileCollidableX() && kvelocityY < 0)
    {
        kvelocityY = 0;
        kaccelerationY = 0;
        jumping = false;
    }

    // Snap to tile if landing
    if (touchingGround && !jumping)
    {
        isGrounded = true;
        kvelocityY = 0;
        kaccelerationY = 0;
        int tileY = (kyPos + wHeight) / map->tile_height;
        kyPos = tileY * map->tile_height - wHeight;
    }

    else if (jumping && touchingGround && !isGrounded)
    {
        jumping = false;
        isGrounded = true;
    }
    else if (!touchingGround)
    {
        isGrounded = false;
    }

    if (touchingWall)
    {
        // Determine overlap
        int tileW = map->tile_width;
        int tileX = kxPos / tileW;
        int tileRightX = (kxPos + wWidth) / tileW;

        if (kvelocityX > 0)
        {
            float overlap = (kxPos + wWidth) - tileRightX * tileW;
            if (overlap > 0)
            {
                applyForce(-overlap * 500, 0);
                kxPos -= overlap;
            }
        }
        else if (kvelocityX < 0)
        {
            float overlap = tileX * tileW - kxPos;
            if (overlap > 0)
            {
                applyForce(overlap * 500, 0);
                kxPos += overlap;
            }
        }

        if (fabs(kvelocityX) < 1.0f)
        {
            kvelocityX = 0;
            kaccelerationX = 0;
        }
    }

    // Update rect based on kPos
    playerRect.x = static_cast<int>(kxPos);
    playerRect.y = static_cast<int>(kyPos);
    playerRect.w = wWidth;
    playerRect.h = wHeight;

    // Update world int pos
    wXPos = playerRect.x;
    wYPos = playerRect.y;

    // Adjust for camera
    SDL_Rect screenRect = {
        playerRect.x - camera.x,
        playerRect.y - camera.y,
        playerRect.w,
        playerRect.h};

    // Animate and draw
    playerTexture.animateSprite(wRenderer, playerTexture.getCols(), playerTexture.getCells(), screenRect, rotate, NULL, flip);
    SDL_RenderDrawRect(wRenderer, &screenRect);
}

void Player::update(float d)
{
    // cout << isGrounded << endl;
    dt = d;
}
bool Player::isTileCollidableY()
{
    if (!map)
    {
        std::cerr << "[ERROR] Map is NULL!\n";
        return false;
    }

    int tileY = (wYPos + wHeight) / map->tile_height;

    // Left and right foot positions
    int tileX1 = wXPos / map->tile_width;
    int tileX2 = (wXPos + wWidth - 1) / map->tile_width;

    cout << "kyPos: " << kyPos << " " << "tileY: " << tileY << " " << "kxPos: " << kxPos << "tileX1: " << tileX1 << "tileX2: " << tileX2 << endl;

    const tmx_layer *layer = map->ly_head;
    while (layer && layer->type != L_LAYER)
    {
        layer = layer->next;
    }

    if (!layer || !layer->content.gids)
    {
        std::cerr << "[ERROR] No valid tile layer or gids found!\n";
        return false;
    }

    bool collidable = false;

    // Check both bottom corners of the player
    for (int tileX : {tileX1, tileX2})
    {
        if (tileX < 0 || tileY < 0 || tileX >= (int)map->width || tileY >= (int)map->height)
            continue;

        int index = tileY * map->width + tileX;
        unsigned int gid = layer->content.gids[index] & TMX_FLIP_BITS_REMOVAL;

        if (gid == 0)
            continue;

        const tmx_tile *tile = tmx_get_tile(map, gid);
        if (!tile || !tile->properties)
            continue;

        tmx_property *prop = tmx_get_property(tile->properties, "collidable");
        if (prop && prop->type == PT_BOOL && prop->value.boolean)
        {
            collidable = true;
            break;
        }
    }

    return collidable;
}
bool Player::isTileCollidableX()
{
    if (!map)
    {
        std::cerr << "[ERROR] Map is NULL!\n";
        return false;
    }

    // Top and bottom Y positions
    int tileY1 = wYPos / map->tile_height;
    int tileY2 = (wYPos + wHeight - 1) / map->tile_height;

    // Left and right X positions
    int tileX1 = wXPos / map->tile_width;
    int tileX2 = (wXPos + wWidth - 1) / map->tile_width;

    cout << "[DEBUG] kyPos: " << kyPos << ", kxPos: " << kxPos
         << ", tileY1: " << tileY1 << ", tileY2: " << tileY2
         << ", tileX1: " << tileX1 << ", tileX2: " << tileX2 << endl;

    const tmx_layer *layer = map->ly_head;
    while (layer && layer->type != L_LAYER)
    {
        layer = layer->next;
    }

    if (!layer || !layer->content.gids)
    {
        std::cerr << "[ERROR] No valid tile layer or gids found!\n";
        return false;
    }

    // We'll check the sides of the player at top and bottom
    bool collidable = false;

    for (int tileY : {tileY1, tileY2})
    {
        for (int tileX : {tileX1, tileX2})
        {
            if (tileX < 0 || tileY < 0 || tileX >= (int)map->width || tileY >= (int)map->height)
                continue;

            int index = tileY * map->width + tileX;
            unsigned int gid = layer->content.gids[index] & TMX_FLIP_BITS_REMOVAL;

            if (gid == 0)
                continue;

            const tmx_tile *tile = tmx_get_tile(map, gid);
            if (!tile || !tile->properties)
                continue;

            tmx_property *prop = tmx_get_property(tile->properties, "collidable");
            if (prop && prop->type == PT_BOOL && prop->value.boolean)
            {
                collidable = true;
                cout << "[DEBUG] Collidable tile detected at (" << tileX << "," << tileY << ")\n";
                return true;
            }
        }
    }

    cout << "isCollidableX = " << collidable << endl;
    return collidable;
}

void Player::jump()
{
    if (!jumping)
    {
        wYVel += wJumpPower;
        jumping = true;
    }
}
