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

struct ConfigResponse : ControllerResponse {
    CameraWidget *widget = nullptr;
};

struct GetConfigResponse : ControllerResponse {
    std::string value;
    std::vector<std::string> values;
    bool read_only = false;
};

struct CameraPreviewResponse : ControllerResponse {
    unsigned long int size;
    std::vector<char> data;
};

class CameraController {
private:
    Camera *_camera = nullptr;
    GPContext *_context = nullptr;
    
    std::mutex _lock;

    static ConfigResponse get_config_internal(CameraWidget *config, const std::string &name);

public:
    ControllerResponse connect();

    ControllerResponse disconnect();

    ControllerResponse set_config_item(const std::string &name, const std::string &value);

    GetConfigResponse get_config_item(const std::string &name);

    CameraPreviewResponse capture_preview();

    ControllerResponse capture_image();
};