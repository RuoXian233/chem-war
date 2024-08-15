#include "utils.hpp"
#include <SDL2/SDL.h>


namespace chem_war {
    [[noreturn]] void Fatal(const std::string &msg, bool internal) {
        std::cerr << "Fatal: " << msg << std::endl;
        if (internal) {
            std::cerr << "Cause: " << SDL_GetError() << std::endl;
        }
        assert(false);
    }


    Vec2 operator+(const Vec2 &self, const Vec2 &other) {
        return Vec2(self.x + other.x, self.y + other.y);
    }

    void operator+=(Vec2 &self, const Vec2 &other) {
        self.x += other.x;
        self.y += other.y;
    }

    Vec2 operator*(const Vec2 &self, float other) {
        return Vec2((int) (self.x * other), (int) (self.y * other));
    }

    Vec2 operator*(float self, const Vec2 &other) {
        return Vec2((int) (other.x * self), (int) (other.y * self));
    }

    void operator*=(Vec2 &self, float other) {
        self.x = (int) (self.x * other);
        self.y = (int) (self.y * other);
    }

    SDL_Rect Vec2::CreateRect(const Vec2 &a, const Vec2 &b) {
        SDL_Rect r = { (int) a.x, (int) a.y, (int) b.x, (int) b.y };
        return r;
    }

    std::vector<std::string> StringSplit(const std::string &str, const std::string &splitter) {
        std::vector<std::string> tokens;
        if (str != splitter && str.length() >= splitter.length()) {
            std::string splitter = splitter;        
            std::string str = str;
            str += splitter;

            size_t pos = str.find(splitter);
            int step = splitter.length();

            while (pos != str.npos) {
                std::string temp = str.substr(0, pos);
                tokens.emplace_back(temp);
                str = str.substr(pos + step, str.length());
                pos = str.find(splitter);
            
            }
            return tokens;
        } else {
            tokens.push_back(str);
            return tokens;
        }
    }
} 
