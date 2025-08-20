
#include "camera.hpp"
#include <cmath>

// ================== Vec2 implementation ==================
Vec2 Vec2::operator-(const Vec2 &other) const {
  return {x - other.x, y - other.y};
}

Vec2 Vec2::operator+(const Vec2 &other) const {
  return {x + other.x, y + other.y};
}

Vec2 Vec2::operator*(float scalar) const { return {x * scalar, y * scalar}; }

float Vec2::magnitude() const { return std::sqrt(x * x + y * y); }

Vec2 Vec2::normalized() const {
  float mag = magnitude();
  return (mag > 0.0f) ? Vec2{x / mag, y / mag} : Vec2{0.0f, 0.0f};
}

// ================== Helpers ==================
float ClampBottom(float val, float minVal) {
  return (val < minVal) ? minVal : val;
}

// ================== Camera implementation ==================
Camera::Camera(int x, int y, int w, int h) {
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;
}

void Camera::Update(const Vec2 &target, float followHeight, float followRadius,
                    float followSpeed, float deltaTime) {
  Vec2 cameraPos = {static_cast<float>(rect.x), static_cast<float>(rect.y)};

  // Use rect.w/h directly (they’re already scaled before calling Update)
  Vec2 targetPos = {target.x - rect.w / 2.0f,
                    target.y - rect.h / 2.0f + followHeight};

  Vec2 diff = targetPos - cameraPos;
  Vec2 normal = diff.normalized();

  float dist = (normal.x != 0.0f) ? diff.x / normal.x : diff.magnitude();
  float moveDist = ClampBottom(dist, followRadius) - followRadius;

  Vec2 newPos = cameraPos + normal * moveDist * followSpeed * deltaTime;

  rect.x = static_cast<int>(newPos.x);
  rect.y = static_cast<int>(newPos.y);
}
