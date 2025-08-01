#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "globals.hpp"
#include <iostream>
#include "map.hpp"
#include "gravity.hpp"
#include "texture.hpp"
#include "timer.hpp"
#include <tmx.h>
using namespace std;

class Player
{
    // width, height of player
    int wWidth, wHeight;

    // weight of the player
    double wWeight;

    // max velocity of the player
    double wMaxVel;

    // ATK of the player
    double wAtk;

    // health of the player
    int wHealth;

    // lives of the player
    int wLives;

    // the x position anf=d y position of player w.r.t screen
    int wXPos;
    int wYPos;

    // the x and y velocities
    int wXVel;
    int wYVel;

    // velocity due to gravity
    double wGravityVel;

    // rectangle of the player
    SDL_Rect playerRect = {wXPos, wYPos, wWidth, wHeight};

    // flip
    SDL_RendererFlip flip;

    // detlatime
    float dt;

    // rotate
    double rotate;

    // jump power
    double wJumpPower;

    // player map
    tmx_map *map;

    // upvelocity
    double upVel;

public:
    // constructor
    Player();

    // getters and setters
    int width();

    int height();

    double weight();

    double maxVel();

    double atk();

    int health();

    int lives();

    int xPos();

    int yPos();

    int xVel();

    int yVel();

    double jumpPower();

    double gravityVel();

    auto getCollider();

    auto getMap();

    bool getUpVel();

    void setWidth(int w);

    void setHeight(int h);

    void setWeight(double w);

    void setMaxVel(double m);

    void setAtk(double a);

    void setHealth(int h);

    void setLives(int l);

    void setXPos(int x);

    void setYPos(int y);

    void setXVel(int x);

    void setYVel(int y);

    void setCollider(SDL_Rect p);

    void setFlip(SDL_RendererFlip f);

    void setRotate(double a);

    void setGravityVel(double g);

    void setJumpPower(double p);

    void setMap(tmx_map *map);

    void setUpVel(bool u);

    // move the player
    void moveRender();

    // update the time;
    void update(float d);

    // the texture of the player
    WTexture playerTexture;

    // gravity
    Gravity gravity;

    // collision detection
    bool isTileCollidable();

    // jump
    void jump();
};
#endif