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

        inline float Length() const {
            return std::sqrt(this->x * this->x + this->y * this->y);
        }

        static Vec2 Rotate(const Vec2 &v, float angle);
        static SDL_Rect CreateRect(const Vec2 &a, const Vec2 &b);
        static SDL_FRect CreateFRect(const Vec2 &a, const Vec2 &b);
    };

    struct QuadTree final {
        struct ObjectWithRect {
            std::string id;
            void *object;
            Vec2 pos;
            Vec2 size;
        };

        std::vector<ObjectWithRect> objects;
        std::array<QuadTree *, 4> children;
        int level;
        Vec2 pos;
        Vec2 size;

        const int maxObjects = 1;
        const int maxLevels = 5;

        QuadTree(int pLevel, const Vec2 &boundPos, const Vec2 &boundSize);
        void Clear();
        std::vector<int> GetIndexes(const Vec2 &rectPos, const Vec2 &rectSize);
        void Insert(const ObjectWithRect &object, const Vec2 &objectPos, const Vec2 &objectSize);
        void Split();
        std::vector<ObjectWithRect>Retrieve(std::vector<ObjectWithRect> &result, const Vec2 &rectPos, const Vec2 &rectSize);
    };  

    enum class Direction {
        Up, Down, Left, Right
    };

    Vec2 operator+(const Vec2 &self, const Vec2 &other);
    void operator+=(Vec2 &self, const Vec2 &other);
    Vec2 operator-(const Vec2 &self, const Vec2 &other);
    void operator-=(Vec2 &self, const Vec2 &other);
    Vec2 operator*(const Vec2 &self, float other);
    void operator*=(Vec2 &self, float other);
    Vec2 operator*(float self, const Vec2 &other);

    std::vector<std::string> StringSplit(const std::string &str, const std::string &splitter);
    bool PointInRect(const Vec2 &point, const Vec2 &xy, const Vec2 &wh);
}
