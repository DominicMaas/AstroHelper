#pragma once

#include <httplib.h>
#include "../camera_controller.h"

class HTTP {
private:
    CameraController *_controller;
    httplib::Server _server;

public:
    explicit HTTP(CameraController *controller);

    void listen();
};