#pragma once

#include <cmath>

// --- Vec2 Definition ---
// A 2D Vector/Point structure
struct Vec2 {
    float x;
    float y;

    // Operators defined in the .cpp file
    Vec2 operator-(const Vec2 &other) const;
    Vec2 operator+(const Vec2 &other) const;
    Vec2 operator*(float scalar) const;
    
    // Utility functions defined in the .cpp file
    float magnitude() const;
    Vec2 normalized() const;
};

// --- Rect Definition ---
// The integer rectangle used for screen drawing/culling
struct Rect {
    int x;
    int y;
    int w;
    int h;
};

// --- Camera Class ---
class Camera {
public:
    // This is the integer representation of the camera's viewport,
    // used for rendering and global access (like the original x/y).
    Rect rect; 
    void SetFloatPosition(const Vec2& newPos);
    Camera(int x, int y, int w, int h);
    
    /**
     * @brief Updates the camera's position based on a target and follow parameters.
     * * @param target The world coordinate of the object to follow.
     * @param followHeight Vertical offset from the target center.
     * @param followRadius Distance the target can move before the camera starts catching up.
     * @param followSpeed How quickly the camera catches up (smoothing factor).
     * @param deltaTime The time elapsed since the last update (for frame-rate independence).
     */
    void Update(const Vec2 &target, float followHeight, float followRadius,
                float followSpeed, float deltaTime);

    // Optional: A helper to get the smooth, float position if needed externally
    Vec2 GetFloatPosition() const { return _position; }

private:
    // **CRUCIAL FIX:** The camera's true, high-precision floating-point position.
    Vec2 _position;
};

// --- Helper Functions Declaration ---
float ClampBottom(float val, float minVal);
