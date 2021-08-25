#pragma once

#include <iostream>
#include <fmt/core.h>
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-result.h>

struct ControllerResponse {
    bool successful = true;
    std::string message = "Default Message";
};

struct GetConfigResponse : ControllerResponse {
    const void *value = nullptr;
    const void **values = nullptr;
};

class CameraController {
private:
    Camera *_camera = nullptr;
    GPContext *_context = nullptr;

public:
    ControllerResponse connect();

    ControllerResponse disconnect();

    ControllerResponse set_config_item(const char *name, const void *value);

    GetConfigResponse get_config_item(const char *name);

    ControllerResponse capture_preview();

    ControllerResponse capture_image();
};