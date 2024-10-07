#include "utils.hpp"


namespace engine::fake3d {
    struct Vec3 {
        float x, y, z;

        Vec3();
        Vec3(float x, float y, float z);

        Vec3 Rotate(const Vec3 &axis, float theta);
        std::string ToString();
        float Length();
    };

    struct Camera3D {
        Vec3 pos;
    };  

    Vec3 operator+(const Vec3 &a, const Vec3 &b);
    float GetViewingDistance(float fov, float screenWidth);

}
