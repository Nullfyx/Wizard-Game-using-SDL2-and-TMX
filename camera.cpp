#include "camera.hpp"
#include <algorithm> // For std::round
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
  // Return a zero vector if magnitude is zero to prevent division by zero
  return (mag > 0.0f) ? Vec2{x / mag, y / mag} : Vec2{0.0f, 0.0f};
}

// ================== Helpers ==================
float ClampBottom(float val, float minVal) {
  return (val < minVal) ? minVal : val;
}
void Camera::SetFloatPosition(const Vec2 &newPos) {
  _position = newPos;

  // Optional: Keep the rect updated if other parts rely on it
  rect.x = static_cast<int>(std::round(_position.x));
  rect.y = static_cast<int>(std::round(_position.y));
}
// ================== Camera implementation ==================
Camera::Camera(int x, int y, int w, int h) {
  rect.x = x;
  rect.y = y;
  rect.w = w;
  rect.h = h;

  // Initialize the float position from the starting integer position
  _position.x = static_cast<float>(x);
  _position.y = static_cast<float>(y);
}

void Camera::Update(const Vec2 &target, float followHeight, float followRadius,
                    float followSpeed, float deltaTime) {

  // Read the current float position
  Vec2 cameraPos = _position;

  // Calculate the ideal target position (center of the screen + vertical
  // offset)
  Vec2 targetPos = {target.x - rect.w / 2.0f,
                    target.y - rect.h / 2.0f + followHeight};

  Vec2 diff = targetPos - cameraPos;
  float distance = diff.magnitude();

  if (distance > 0.0f) {
    Vec2 direction = diff.normalized();

    // This calculates the distance *outside* the follow radius.
    // It will be negative if the target is inside the radius,
    // which causes the camera to move toward the target to "re-center" it
    // slightly, ensuring it doesn't drift too far in one direction.
    float excess = distance - followRadius;

    // Exponential smoothing factor: frame-rate independent
    float smoothFactor = 1.0f - std::exp(-followSpeed * deltaTime);

    // Calculate the smooth movement vector. No movement thresholds here!
    Vec2 move = direction * excess * smoothFactor;

    // Apply movement to the high-precision float position
    cameraPos = cameraPos + move;
  }

  // 1. Store the new float position
  _position = cameraPos;

  // 2. Update the integer Rect and global x/y using **rounding** (the jitter
  // fix!)
  // rect.x = static_cast<int>(std::round(_position.x));
  // rect.y = static_cast<int>(std::round(_position.y));

  // Note: If you have separate 'x' and 'y' members for global use,
  // you would update them here as well:
  // this->x = rect.x;
  // this->y = rect.y;
}
