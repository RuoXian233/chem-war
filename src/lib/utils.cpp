#include "utils.hpp"
#include <SDL2/SDL.h>


namespace engine {
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

     Vec2 operator-(const Vec2 &self, const Vec2 &other) {
        return Vec2(self.x - other.x, self.y - other.y);
    }

    void operator+=(Vec2 &self, const Vec2 &other) {
        self.x += other.x;
        self.y += other.y;
    }

    void operator-=(Vec2 &self, const Vec2 &other) {
        self.x -= other.x;
        self.y -= other.y;
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

    Vec2 operator-(const Vec2 &self) {
        return Vec2(-self.x, -self.y);
    }

    float operator*(const Vec2 &self, const Vec2 &other) {
        return self.x * other.x + self.y * other.y;
    }

    SDL_Rect Vec2::CreateRect(const Vec2 &a, const Vec2 &b) {
        SDL_Rect r = { (int) a.x, (int) a.y, (int) b.x, (int) b.y };
        return r;
    }
    
    SDL_FRect Vec2::CreateFRect(const Vec2 &a, const Vec2 &b) {
        SDL_FRect r = { a.x, a.y, b.x, b.y };
        return r;
    }

    Vec2 Vec2::Rotate(const Vec2 &v, float angle) {
        float rad = DEG_TO_RAD(angle);
        return Vec2(
            cosf(rad) * v.x - sinf(rad) * v.y,
            sinf(rad) * v.x + cosf(rad) * v.y
        );
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

    bool PointInRect(const Vec2 &point, const Vec2 &xy, const Vec2 &wh) {
        SDL_FRect r = { xy.x, xy.y, wh.x, wh.y };
        SDL_FPoint p = { point.x, point.y };
        return SDL_PointInFRect(&p, &r);
    }


    QuadTree::QuadTree(int pLevel, const Vec2 &boundPos, const Vec2 &boundSize) {
        this->level = pLevel;
        this->pos = boundPos;
        this->size = boundSize;
        this->objects = {};
        this->children = { nullptr, nullptr, nullptr, nullptr };
    }

    void QuadTree::Clear() {
        this->objects.clear();
        for (auto &child : this->children) {
            if (child) {
                child->Clear();
                child = nullptr;
            }
        }
    }

    std::vector<int> QuadTree::GetIndexes(const Vec2 &rectPos, const Vec2 &rectSize) {
        std::vector<int> indexes;

        Vec2 midPoint = this->pos + this->size * 0.5;

        bool topQuadrant = rectPos.y >= midPoint.y;
        bool bottomQuadrant = (rectPos.y - rectSize.y) <= midPoint.y;
        bool topAndBottomQuadrant = rectPos.y + rectSize.y + 1 >= midPoint.y && rectPos.y + 1 <= midPoint.y;

        if (topAndBottomQuadrant) {
            topQuadrant = false;
            bottomQuadrant = false;
        }

        if (rectPos.x + rectSize.x + 1 >= midPoint.x && rectPos.x - 1 <= midPoint.x) {
            if (topQuadrant) {
                indexes.push_back(2);
                indexes.push_back(3);
            } else if (bottomQuadrant) {
                indexes.push_back(0);
                indexes.push_back(1);
            } else if (topAndBottomQuadrant) {
                indexes.push_back(0);
                indexes.push_back(1);
                indexes.push_back(2);
                indexes.push_back(3);
            }
        } else if (rectPos.x + 1 >= midPoint.x) {
            if (topQuadrant) {
                indexes.push_back(3);
            } else if (bottomQuadrant) {
                indexes.push_back(0);
            } else if (topAndBottomQuadrant) {
                indexes.push_back(3);
                indexes.push_back(0);
            }
        } else if (rectPos.x - rectSize.x <= midPoint.x) {
            if (topQuadrant) {
                indexes.push_back(2);
            } else if (bottomQuadrant) {
                indexes.push_back(1);
            } else if (topAndBottomQuadrant) {
                indexes.push_back(2);
                indexes.push_back(1);
            }
        } else {
            indexes.push_back(-1);
        }
        return indexes;
    }

    void QuadTree::Split() {
        int subWidth =  this->size.x / 2;
        int subHeight = this->size.y / 2;
        int x = this->pos.x;
        int y = this->pos.y;
        
        this->children[0] = new QuadTree(level + 1, Vec2(x + subWidth, y), Vec2(subWidth, subHeight));
        this->children[1] = new QuadTree(level + 1, Vec2(x, y), Vec2(subWidth, subHeight));
        this->children[2] = new QuadTree(level + 1, Vec2(x, y + subHeight), Vec2(subWidth, subHeight));
        this->children[3] = new QuadTree(level + 1, Vec2(x + subWidth, y + subHeight), Vec2(subWidth, subHeight));
    }

    void QuadTree::Insert(const ObjectWithRect &object, const Vec2 &objectPos, const Vec2 &objectSize) {
        if (this->children[0]) {
            auto indexes = this->GetIndexes(objectPos, objectSize);

            for (int ii = 0; ii < indexes.size(); ii++) {
                int index = indexes[ii];
                if (index != -1) {
                    this->children[index]->Insert(object, objectPos, objectSize);
                    return;
                }
            }
        }
        
        this->objects.push_back(object);
        
        if (this->objects.size() > this->maxObjects && this->level < this->maxLevels) {
            if (!this->children[0]) {
                this->Split();
            }
            
            int i = 0;
            while (i < objects.size()) {
                auto obj = objects[i];
                auto indexes = GetIndexes(obj.pos, obj.size);
                for (int ii = 0; ii < indexes.size(); ii++) {
                    int index = indexes[ii];
                    if (index != -1) {
                        this->children[index]->Insert(obj, obj.pos, obj.size);
                        objects.erase(objects.begin() + i);
                    } else {
                        i++;
                    }
                }
            }
        }
    }

    std::vector<QuadTree::ObjectWithRect> QuadTree::Retrieve(std::vector<ObjectWithRect > &result, const Vec2 &rectPos, const Vec2 &rectSize) {
        auto  indexes = GetIndexes(rectPos, rectSize);

        for(int ii = 0; ii < indexes.size(); ii++) {
            int index = indexes[ii];
            if (index != -1 && this->children[0]) {
                this->children[index]->Retrieve(result, rectPos, rectSize);
            }    
                
            for (auto o : this->objects) {
                result.emplace_back(o);
            }
        }
        
        return result;
    }
} 
