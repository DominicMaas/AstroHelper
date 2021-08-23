//
// Created by Dominic Maas on 23/08/21.
//

#include "CameraController.h"

ControllerResponse CameraController::connect() {
    // TODO: What happens if connect is called twice? Clean up old resources

    // Required context and camera init
    this->_context = gp_context_new();

    // Attempt to init the camera
    gp_camera_new(&this->_camera);
    int init_res = gp_camera_init(this->_camera, this->_context);
    if (init_res != 0) {
        auto message = fmt::format("Unable to connect to camera. Result: {}", gp_result_as_string(init_res));

        fmt::print("{}\n", message);
        return ControllerResponse { false, message };
    }

    CameraText cam_text;
    gp_camera_get_about(this->_camera, &cam_text, this->_context);

    std::cout << "Camera Text: " << cam_text.text << std::endl;
    return ControllerResponse {  };
}

ControllerResponse CameraController::disconnect() {
    if (this->_camera != nullptr) {
        gp_camera_exit(this->_camera, this->_context);
    }

    return ControllerResponse {  };
}

ControllerResponse CameraController::set_config_item(const char *name, const void *value) {
    // Keep track of return codes
    int res;

    // Get the config
    CameraWidget *config;
    res = gp_camera_get_config(this->_camera, &config, this->_context);
    if (res != 0) {
        auto message = fmt::format("Unable to get camera config. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);
        return ControllerResponse { false, message };
    }

    // Get the widget
    CameraWidget *widget;
    res = gp_widget_get_child_by_name(config, name, &widget);
    if (res != 0) {
        auto message = fmt::format("Unable to get specified camera config ({}). Result: {}", name, gp_result_as_string(res));

        fmt::print("{}\n", message);
        return ControllerResponse { false, message };
    }

    // Attempt to set the value
    res = gp_widget_set_value(widget, value);
    if (res != 0) {
        auto message = fmt::format("Unable to set camera config ({}) value. Result: {}", name, gp_result_as_string(res));

        fmt::print("{}\n", message);
        return ControllerResponse { false, message };
    }

    // Attempt to set the config
    res = gp_camera_set_config(this->_camera, config, this->_context);
    if (res != 0) {
        auto message = fmt::format("Unable to set camera config. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);
        return ControllerResponse { false, message };
    }

    return ControllerResponse {  };
}

GetConfigResponse CameraController::get_config_item(const char *name) {
    return GetConfigResponse {  };
}


