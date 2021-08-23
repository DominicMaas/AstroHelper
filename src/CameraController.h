#pragma once

#include <iostream>
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-result.h>


class CameraController {
private:
    Camera *_camera = nullptr;
    GPContext *_context = nullptr;

public:
    bool connect();
    bool disconnect();
};