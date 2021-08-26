#pragma once

#include <iostream>
#include <vector>
#include <fmt/core.h>
#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-result.h>

struct ControllerResponse {
    bool successful = true;
    std::string message = "Default Message";
};

struct GetConfigResponse : ControllerResponse {
    std::string value;
    std::vector<std::string> values;
};

class CameraController {
private:
    Camera *_camera = nullptr;
    GPContext *_context = nullptr;

public:
    ControllerResponse connect();

    ControllerResponse disconnect();

    ControllerResponse set_config_item(const std::string& name, const void *value);

    GetConfigResponse get_config_item(const std::string& name);

    ControllerResponse capture_preview();

    ControllerResponse capture_image();
};