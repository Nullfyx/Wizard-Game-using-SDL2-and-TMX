#ifndef MAP_HPP
#define MAP_HPP
#include "mapglobal.hpp"
#include <SDL2/SDL.h>
#include <tmx.h>
#include <cstring>
#include "globals.hpp"
#include <iostream>
#include "player.hpp"
#include "Kinematics.hpp"
#include "particleSystem.hpp"
#include "lightSystem.hpp"
using namespace std;

// Renderer used globally (defined elsewhere)
extern SDL_Renderer *wRenderer;
// Core render functions
void set_color(int color);
void render_map(tmx_map *map, float dt);
void draw_all_layers(tmx_map *map, tmx_layer *layers, float dt);

// Layer and object drawing
void draw_objects(tmx_object_group *objgr);
void draw_image_layer(tmx_image *image);
void draw_layer(tmx_map *map, tmx_layer *layer, float dt);

// Tile and shape drawing
void draw_tile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
               unsigned int dx, unsigned int dy, float opacity, unsigned int flags);
void draw_polyline(double **points, double x, double y, int pointsc);
void draw_polygon(double **points, double x, double y, int pointsc);


#endif

