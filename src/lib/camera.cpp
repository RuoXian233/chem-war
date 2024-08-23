#include "camera.h"

using namespace engine;

CameraState Camera::currentState;

void Camera::Initalize() {
    Camera::currentState = CameraState {
        false,
        Vec2(0, 0),
        Vec2(WINDOW_WIDTH, WINDOW_HEIGHT),
        1.0f
    };
}


void Camera::Update(float dt) {

}

CameraState &Camera::GetState() {
    return Camera::currentState;
}

void Camera::Finalize() {

}

void Camera::Translate(const Vec2 &delta) {
    Camera::currentState.pos += delta;
} 

void Camera::Zoom(float zoom) {
    Camera::currentState.zoom += zoom;
}

void Camera::SetViewPort(const Vec2 &port) {
    Camera::currentState.viewPort = port;
}

void Camera::Enabled(bool val) {
    Camera::currentState.enabled = val;
}

bool Camera::Enabled() {
    return Camera::currentState.enabled;
}

void Camera::Reset() {
    Camera::currentState = CameraState {
        true,
        Vec2(0, 0),
        Vec2(WINDOW_WIDTH, WINDOW_HEIGHT),
        1.0f
    };
}
