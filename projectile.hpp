#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP
#include "mapglobal.hpp"
#include "Kinematics.hpp"
#include "bounding_box.hpp"
#include "texture.hpp"
#include "timer.hpp"
#include <SDL2/SDL.h>
    struct projectile{ 
	projectile(int sx, int sy, int tx, int ty);
	SDL_Rect proRect;
	Kinematics physics;
	void update();
	double accDt = 0;
	WTexture texture;
	static int index;
	bool destroyMe = false;
	double angle = 0;
	int power = 2;
	double cooldown = 1;
	Timer timer;
	bool canFire();

    };
#endif
