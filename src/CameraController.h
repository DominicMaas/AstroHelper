#pragma once

#include <iostream>
#include <gphoto2/gphoto2-camera.h>


class CameraController {
private:
    Camera *_camera = nullptr;

public:
    bool connect();
};