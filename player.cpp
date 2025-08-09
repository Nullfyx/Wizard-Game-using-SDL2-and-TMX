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

    // Input debug
    cout << "k " << kxPos << " " << kyPos << endl;
    cout << "p " << wXPos << " " << wYPos << endl;
    cout << "isGrounded (before move): " << isGrounded << endl;
    cout << "moveleft: " << moveLeft << ", moveRight: " << moveRight << ", jump: " << jump << endl;

    bool onGround = false, wallLeft = false, wallRight = false, onCeiling = false, overlapping = false;

    checkCollisionsXY(map, onGround, wallLeft, wallRight, onCeiling, overlapping , playerRect);

    // Apply forces to update kPos
    //apply gravity
    applyForce(0, kgravityConstant * wWeight * dt);
    if(onGround)
    {
	kvelocityY = 0;
//`	applyForce(0, -kgravityConstant * wWeight * dt);
    }
    

const double overlapPushSpeed = 1.0;  // pixels per frame to push player out of overlap

if (overlapping) {
    if (onGround) {
        // Push player up by a small amount to prevent sinking
        kyPos -= overlapPushSpeed;
        kvelocityY = 0; // Stop downward velocity since we are pushing up
        isGrounded = true;  // confirm grounded state
    }
    else if (onCeiling) {
        // Push player down gently when overlapping ceiling
        kyPos += overlapPushSpeed;
        kvelocityY = 0; // stop upward velocity
    }
    else {
        // Overlapping but neither on floor nor ceiling - push up as fallback
        kyPos -= overlapPushSpeed * 0.5;
    }
}
else {
    // No overlapping
    if (onGround) {
        kvelocityY = 0;
        isGrounded = true;
    } else {
        isGrounded = false;
    }
}
    if(jump && onGround)
    {
	applyForce(0, -3500);
    }

if (wallRight) {
     kvelocityX = 0;
    kforceX = kforceX <= 0 ? kforceX : 0;
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
    	kvelocityX *= 0.6;

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
