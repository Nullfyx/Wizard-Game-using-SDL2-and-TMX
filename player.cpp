#include "player.hpp"
Player::Player()
{
    wWidth = 20;
    wHeight = 20;
    wWeight = 1000; // N
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
    wJumpPower = -100; // Newtons
    map = nullptr;
    jumping = false;
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

<<<<<<< HEAD
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
    if (isGrounded && !moveLeft && !moveRight)
    {
        kvelocityX = 0;
        kaccelerationX = 0;
    }

    // Jumping
    if (jump && isGrounded)
        applyForce(0, -4000);

    // Apply gravity if airborne
    if (!isGrounded)
        applyForce(0, kmass * kgravityConstant);
=======
    // Input debug
    cout << "k " << kxPos << " " << kyPos << endl;
    cout << "p " << wXPos << " " << wYPos << endl;
    cout << "isGrounded (before move): " << isGrounded << endl;
    cout << "moveleft: " << moveLeft << ", moveRight: " << moveRight << ", jump: " << jump << endl;

    bool onGround = false, wallLeft = false, wallRight = false, onCeiling = false, overlapping = false;
    checkCollisionsXY(map, kxPos, kyPos, onGround, wallLeft, wallRight, onCeiling, overlapping );
>>>>>>> improve_the_kinematics

    // Apply forces to update kPos
    //apply gravity
    applyForce(0, kgravityConstant * wWeight * dt);
    if(onGround)
    {
	kvelocityY = 0;
	applyForce(0, -kgravityConstant * wWeight * dt);
    }
    
    if(overlapping)
    {
	    kyPos+= 0.1;
    }

    if(jump && onGround)
    {
	applyForce(0, -3500);
    }

    

    //apply motion
    if(moveRight)
    {
	applyForce(1000, 0);
    }

    if(moveLeft)
    {
	applyForce(-1000, 0);
    }
//resistance
    if (!moveLeft && !moveRight)
    {
    	kvelocityX *= 0.8;

    	if (std::abs(kvelocityX) < 0.01)
        kvelocityX = 0;
    }


    if (wallLeft)
    {
    	kvelocityX = 0;
    	kforceX = kforceX > 0 ? kforceX : 0;
	kxPos += 1;
    }
    if(wallRight)
    {
	kvelocityX = 0;
    	kforceX = kforceX <= 0 ? kforceX : 0;
	kxPos -= 1;
    }
    if(onCeiling && !onGround )
    {
	applyForce(0, 3000);    
    }
    move();

<<<<<<< HEAD
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
        // int tileY = (kyPos + wHeight) / map->tile_height;
        // kyPos = tileY * map->tile_height - wHeight;
        // Check if player overlaps the tile vertically
        int bottomY = kyPos + wHeight;
        int tileY = bottomY / map->tile_height;
        int tileTop = tileY * map->tile_height;

        int overlapY = bottomY - tileTop;

        if (touchingGround && !jumping && overlapY > 0 && overlapY <= map->tile_height)
        {
            isGrounded = true;
            kvelocityY = 0;
            kaccelerationY = 0;

            // Snap Y pos exactly to the top of the tile
            kyPos -= overlapY; // pull player up by overlap
        }
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
            cout << "overlap: " << overlap << endl;

            if (overlap > 0)
            {
                applyForce(-overlap * 5, 0);
                kxPos -= overlap;
            }
        }
        else if (kvelocityX < 0)
        {
            float overlap = tileX * tileW - kxPos;
            cout << "overlap: " << overlap << endl;
            if (overlap > 0)
            {
                applyForce(-overlap * 5, 0);
                kxPos += overlap;
            }
        }

        if (fabs(kvelocityX) < 1.0f)
        {
            kvelocityX = 0;
            kaccelerationX = 0;
        }
    }

=======
>>>>>>> improve_the_kinematics
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
        (int)((playerRect.x - camera.x)),
        (int)((playerRect.y - camera.y)),
        (int)(playerRect.w),
  	(int)(playerRect.h)
};
    // Animate and draw
    playerTexture.animateSprite(wRenderer, playerTexture.getCols(), playerTexture.getCells(), screenRect, rotate, NULL, flip);
    SDL_RenderDrawRect(wRenderer, &screenRect);
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
