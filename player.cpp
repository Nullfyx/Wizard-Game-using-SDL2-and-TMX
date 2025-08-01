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
    upVel = false;
    wYPos = 0;
    wXVel = 0;
    wGravityVel = 1;
    wYVel = wGravityVel;
    flip = SDL_FLIP_NONE;
    rotate = 0.00;
    wJumpPower = 100; // N
    map = nullptr;
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

bool Player::getUpVel()
{
    return upVel;
}

double Player::jumpPower()
{
    return wJumpPower;
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

void Player::setUpVel(bool u)
{
    upVel = u;
}
void Player::moveRender()
{
    // check if player is on ground
    isTileCollidable(); // updates upVel internally

    // gravity
    wGravityVel = (wWeight / gravity.g()) * dt * 10;

    // move
    if (!upVel)
    {
        // Add gravity gradually
        wYVel += (wWeight / gravity.g()) * dt * 10;
    }
    else
    {
        wYVel = 0; // Reset on ground
    }
    if (wYVel > wMaxVel)
        wYVel = wMaxVel;

    // Move
    wYPos += wYVel * dt * 100;
    wXPos += wXVel * dt * 100;

    // fix awkward y positioning

    // update rect
    playerRect.x = wXPos;
    playerRect.y = wYPos;
    playerRect.w = wWidth;
    playerRect.h = wHeight;

    // camera offset render
    SDL_Rect screenRect = {
        playerRect.x - camera.x,
        playerRect.y - camera.y,
        playerRect.w,
        playerRect.h};

    playerTexture.animateSprite(wRenderer, playerTexture.getCols(), playerTexture.getCells(), screenRect, rotate, NULL, flip);
    SDL_RenderDrawRect(wRenderer, &screenRect);
}

void Player::update(float d)
{
    // cout << upVel << endl;
    dt = d;
}
bool Player::isTileCollidable()
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

    if (collidable)
    {
        setUpVel(true);
        wYPos = tileY * map->tile_height - wHeight; // Snap to tile
        wYVel = 0;
        return true;
    }
    else
    {
        setUpVel(false);
        return false;
    }
}

void Player::jump()
{
    if (upVel)
    {
        wYPos += -wJumpPower * 40 * dt;
        upVel = false;
    }
}
