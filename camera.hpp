
#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <SDL2/SDL.h>

struct Vec2 {
    float x, y;

    Vec2 operator-(const Vec2& other) const;
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator*(float scalar) const;

    float magnitude() const;
    Vec2 normalized() const;
};

float ClampBottom(float val, float minVal);

class Camera {
public:
    SDL_Rect rect;  // Camera viewport (x, y = top-left, w, h = screen size)

    Camera(int x, int y, int w, int h);

    void Update(const Vec2& target, float followHeight, float followRadius, float followSpeed, float deltaTime);
};

#endif // CAMERA_HPP
