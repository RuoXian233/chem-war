#include "fake3d.h"

using namespace engine::fake3d;


Vec3::Vec3() : Vec3::Vec3(0.0f, 0.0f, 0.0f) {}

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

std::string Vec3::ToString() {
    return std::format("Vec3({}, {}, {})", x, y, z);
}


Vec3 operator+(const Vec3 &a, const Vec3 &b) {
    return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 Vec3::Rotate(const Vec3 &axis, float theta) {
    Vec3 rotated;
    rotated.x = (axis.x * axis.x * (1 - cos(theta)) + cos(theta)) * this->x +
                (axis.x * axis.y * (1 - cos(theta)) - axis.z * sin(theta)) * this->y + 
                (axis.x * axis.z * (1 - cos(theta)) + axis.y * sin(theta)) * this->z;
    rotated.y = (axis.x * axis.y * (1 - cos(theta)) + axis.z * sin(theta)) * this->x + 
                (axis.y * axis.y * (1 - cos(theta)) + cos(theta)) * this->y +
                (axis.y * axis.z * (1 - cos(theta)) - axis.x * sin(theta)) * this->z;
    rotated.z = (axis.x * axis.z * (1 - cos(theta)) - axis.y * sin(theta)) * this->x +
                (axis.y * axis.z * (1 - cos(theta)) + axis.x * sin(theta)) * this->y +
                (axis.z * axis.z * (1 - cos(theta)) + cos(theta)) * this->z;
    return rotated;
}

float Vec3::Length() {
    return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}
