// src/engine/core/Types.hpp

#pragma once
#include <cstdint>
#include <cmath>

namespace engine {
    using EntityID = uint32_t;
    
    struct Vector2 {
        float x = 0.0f, y = 0.0f;
        
        Vector2() = default;
        Vector2(float x, float y) : x(x), y(y) {}
        
        Vector2 operator+(const Vector2& other) const {
            return Vector2(x + other.x, y + other.y);
        }
        
        Vector2 operator-(const Vector2& other) const {
            return Vector2(x - other.x, y - other.y);
        }
        
        Vector2 operator*(float scalar) const {
            return Vector2(x * scalar, y * scalar);
        }
        
        float Length() const {
            return std::sqrt(x * x + y * y);
        }
        
        Vector2 Normalized() const {
            float len = Length();
            if (len < 0.001f) return Vector2(0, 0);
            return Vector2(x / len, y / len);
        }
    };
}
