#pragma once

#include "consts.h"
#include <iostream>
#include <cassert>
#include <format>
#include <cmath>
#include <SDL2/SDL.h>
#include <vector>


namespace chem_war {
    bool utils_MapHasKey(const auto &map, const auto &key) {
        return (map.find(key) != map.end());
    }
    [[noreturn]] void Fatal(const std::string &msg, bool internal = true);

    struct Vec2 final {
        float x;
        float y;

        Vec2() : Vec2(0, 0) {}
        Vec2(float x, float y) : x(x), y(y) {}

        inline std::string ToString() const {
            return std::format("({}, {})", this->x, this->y);
        }

        inline float length() const {
            return std::sqrt(this->x * this->x + this->y * this->y);
        }
    
        static SDL_Rect CreateRect(const Vec2 &a, const Vec2 &b);
    };

    enum class Direction {
        Up, Down, Left, Right
    };

    Vec2 operator+(const Vec2 &self, const Vec2 &other);
    void operator+=(Vec2 &self, const Vec2 &other);
    Vec2 operator*(const Vec2 &self, float other);
    void operator*=(Vec2 &self, float other);
    Vec2 operator*(float self, const Vec2 &other);

    std::vector<std::string> StringSplit(const std::string &str, const std::string &splitter);
}
