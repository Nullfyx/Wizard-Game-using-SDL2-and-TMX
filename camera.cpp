
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
  Vec2 targetPos = {target.x - rect.w / 2.0f,
                    target.y - rect.h / 2.0f + followHeight};

  Vec2 diff = targetPos - cameraPos;
  float distance = diff.magnitude();

  // Only move if distance exceeds followRadius + small epsilon
  const float epsilon = 0.5f; // small buffer to avoid jitter
  if (distance > followRadius + epsilon) {
    Vec2 direction = diff.normalized();
    float excess = distance - followRadius;

    // Smooth movement
    float smoothFactor = 1.0f - std::exp(-followSpeed * deltaTime);
    Vec2 move = direction * excess * smoothFactor;

    // Apply movement only if significant
    if (move.magnitude() > epsilon) {
      cameraPos = cameraPos + move;
    }
  }

  rect.x = static_cast<int>(cameraPos.x);
  rect.y = static_cast<int>(cameraPos.y);
}
