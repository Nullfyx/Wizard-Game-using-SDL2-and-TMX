#include "map.hpp"

void set_color(int color)
{
    tmx_col_bytes col = tmx_col_to_bytes(color);
    SDL_SetRenderDrawColor(wRenderer, col.r, col.g, col.b, col.a);
    // SDL_SetRenderDrawColor(wRenderer, 255, 0, 0, 255);
}

void render_map(tmx_map *map)
{
    set_color(map->backgroundcolor);
    draw_all_layers(map, map->ly_head);
}

void draw_all_layers(tmx_map *map, tmx_layer *layers)
{
    while (layers)
    {
        if (layers->visible)
        {

            if (layers->type == L_GROUP)
            {
                draw_all_layers(map, layers->content.group_head); // recursive call
            }
            else if (layers->type == L_OBJGR)
            {
                draw_objects(layers->content.objgr); // Function to be implemented
            }
            else if (layers->type == L_IMAGE)
            {
                draw_image_layer(layers->content.image); // Function to be implemented
            }
            else if (layers->type == L_LAYER)
            {
                draw_layer(map, layers); // Function to be implemented
            }
        }
        layers = layers->next;
    }
}

void draw_image_layer(tmx_image *image)
{
    SDL_Rect dim;
    dim.x = dim.y = 0;

    SDL_Texture *texture = (SDL_Texture *)image->resource_image; // Texture loaded by libTMX
    SDL_QueryTexture(texture, NULL, NULL, &(dim.w), &(dim.h));
    SDL_RenderCopy(wRenderer, texture, NULL, &dim);
}

void draw_layer(tmx_map *map, tmx_layer *layer)
{
    unsigned long i, j;
    unsigned int gid, x, y, w, h, flags;
    float op;
    tmx_tileset *ts;
    tmx_image *im;
    void *image;
    op = layer->opacity;
    for (int i = map->height - 1; i >= 0; --i)
    {
        for (int j = 0; j < map->width; ++j)
        {
            gid = (layer->content.gids[(i * map->width) + j]) & TMX_FLIP_BITS_REMOVAL;
            if (gid < map->tilecount && map->tiles[gid] && map->tiles[gid] != NULL)
            {
                ts = map->tiles[gid]->tileset;
                im = map->tiles[gid]->image;
                x = map->tiles[gid]->ul_x;
                y = map->tiles[gid]->ul_y;
                w = ts->tile_width;
                h = ts->tile_height;
                if (im)
                {
                    image = im->resource_image;
                }
                else
                {
                    image = ts->image->resource_image;
                }
                flags = (layer->content.gids[(i * map->width) + j]) & ~TMX_FLIP_BITS_REMOVAL;
                draw_tile(image, x, y, w, h, j * ts->tile_width, i * ts->tile_height, op, flags); // Function to be implemented
            }
        }
    }
}

void draw_tile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
               unsigned int dx, unsigned int dy, float opacity, unsigned int flags)
{
    SDL_Rect src_rect, dest_rect;
    src_rect.x = sx;
    src_rect.y = sy;
    src_rect.w =sw;
    src_rect.h = sh;
    dest_rect.x = (dx - camera.x);
    dest_rect.y = (dy - camera.y);
    dest_rect.h = sh;
    dest_rect.w = sw;

    SDL_RenderCopy(wRenderer, (SDL_Texture *)image, &src_rect, &dest_rect);
}

void draw_polyline(double **points, double x, double y, int pointsc)
{
    int i;
    for (i = 1; i < pointsc; i++)
    {
        SDL_RenderDrawLine(wRenderer, x + points[i - 1][0], y + points[i - 1][1], x + points[i][0], y + points[i][1]);
    }
}

void draw_polygon(double **points, double x, double y, int pointsc)
{
    draw_polyline(points, x, y, pointsc);
    if (pointsc > 2)
    {
        SDL_RenderDrawLine(wRenderer, x + points[0][0], y + points[0][1], x + points[pointsc - 1][0], y + points[pointsc - 1][1]);
    }
}

void draw_objects(tmx_object_group *objgr)
{
    SDL_Rect rect;
    set_color(objgr->color);
    tmx_object *head = objgr->head;
    while (head)
    {
        if (head->visible)
        {
            if (head->obj_type == OT_SQUARE)
            {
                rect.x = (head->x - camera.x)*4.0;
                rect.y = (head->y - camera.y)*4.0;
                rect.w = head->width * 4.0;
                rect.h = head->height * 4.0;
                SDL_RenderDrawRect(wRenderer, &rect);
            }
            else if (head->obj_type == OT_POLYGON)
            {
                draw_polygon(head->content.shape->points, head->x, head->y, head->content.shape->points_len);
            }
            else if (head->obj_type == OT_POLYLINE)
            {
                draw_polyline(head->content.shape->points, head->x, head->y, head->content.shape->points_len);
            }
            else if (head->obj_type == OT_ELLIPSE)
            {
                /* FIXME: no function in SDL2 */
            }
        }
        head = head->next;
    }
}
// bool is_tile_collidable()
// {
//     int tileX = wXPos / map->tile_width;
//     int tileY = wYPos / map->tile_height;
//     unsigned int tile_index = tileY * map->width + tileX;

//     tmx_layer *layer = map->layers[0];
//     if (layer->type != L_LAYER || !layer->visible)
//         return false;

//     unsigned int gid = layer->content.gids[tile_index];
//     if (!gid)
//         return false;

//     const tmx_tile *tile = map->tiles[gid];
//     if (!tile || !tile->properties)
//         return false;

//     tmx_property *prop = tmx_get_property(tile->properties, "collidable");
//     if (prop && prop->type == PT_BOOL && prop->value.boolean)
//     {
//         std::cout << "Solid tile detected at (" << tileX << ", " << tileY << ")!\n";
//         setYVel(0);
//         setXVel(0);
//         return true;
//     }

//     return false;
// }
