#include "camera_controller.h"

ControllerResponse CameraController::connect() {
    // Required context and camera init
    this->_context = gp_context_new();

    // Attempt to init the camera
    gp_camera_new(&this->_camera);
    int init_res = gp_camera_init(this->_camera, this->_context);
    if (init_res != 0) {
        auto message = fmt::format("Unable to connect to camera. Result: {}", gp_result_as_string(init_res));

        fmt::print("{}\n", message);
        return ControllerResponse{false, message};
    }

    return ControllerResponse{};
}

ControllerResponse CameraController::disconnect() {
    if (this->_camera != nullptr) {
        gp_camera_exit(this->_camera, this->_context);
    }

    return ControllerResponse{};
}

ControllerResponse CameraController::set_config_item(const std::string &name, const void *value) {
    // Start of method, connect
    auto conn_response = this->connect();
    if (!conn_response.successful) {
        return ControllerResponse{false, conn_response.message};
    }

    // Keep track of return codes
    int res;

    // Get the config
    CameraWidget *config;
    res = gp_camera_get_config(this->_camera, &config, this->_context);
    if (res != 0) {
        auto message = fmt::format("Unable to get camera config. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);

        this->disconnect();
        return ControllerResponse{false, message};
    }

    // Get the widget
    CameraWidget *widget;
    res = gp_widget_get_child_by_name(config, name.c_str(), &widget);
    if (res != 0) {
        auto message = fmt::format("Unable to get specified camera config ({}). Result: {}", name,
                                   gp_result_as_string(res));

        fmt::print("{}\n", message);

        this->disconnect();
        return ControllerResponse{false, message};
    }

    // Attempt to set the value
    res = gp_widget_set_value(widget, value);
    if (res != 0) {
        auto message = fmt::format("Unable to set camera config ({}) value. Result: {}", name,
                                   gp_result_as_string(res));

        fmt::print("{}\n", message);

        this->disconnect();
        return ControllerResponse{false, message};
    }

    // Attempt to set the config
    res = gp_camera_set_config(this->_camera, config, this->_context);
    if (res != 0) {
        auto message = fmt::format("Unable to set camera config. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);

        this->disconnect();
        return ControllerResponse{false, message};
    }

    this->disconnect();
    return ControllerResponse{};
}

GetConfigResponse CameraController::get_config_item(const std::string &name) {
    // Start of method, connect
    auto conn_response = this->connect();
    if (!conn_response.successful) {
        return GetConfigResponse{false, conn_response.message};
    }

    // Keep track of return codes
    int res;

    // Get the config
    CameraWidget *config;
    res = gp_camera_get_config(this->_camera, &config, this->_context);
    if (res != 0) {
        auto message = fmt::format("Unable to get camera config. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);

        this->disconnect();
        return GetConfigResponse{false, message};
    }

    // Get the widget
    CameraWidget *widget;
    res = gp_widget_get_child_by_name(config, name.c_str(), &widget);
    if (res != 0) {
        auto message = fmt::format("Unable to get specified camera config ({}). Result: {}", name,
                                   gp_result_as_string(res));

        fmt::print("{}\n", message);

        this->disconnect();
        return GetConfigResponse{false, message};
    }

    const char *value;
    res = gp_widget_get_value(widget, &value);
    if (res != 0) {
        auto message = fmt::format("Unable to get specified camera config value ({}). Result: {}", name,
                                   gp_result_as_string(res));

        fmt::print("{}\n", message);

        this->disconnect();
        return GetConfigResponse{false, message};
    }

    fmt::print("Creating vector of strings...\n");
    auto values = std::vector<std::string>();
    fmt::print("Created!!!\n");
    fmt::print("Value is {}! Now attempting to extract choices...\n", value);

    auto choice_count = gp_widget_count_choices(widget);
    for (int i = 0; i < choice_count; i++) {
        fmt::print("Attempting to extract choice {}!\n", i);
        const char *raw_choice;
        gp_widget_get_choice(widget, i, &raw_choice);
        values.emplace_back(raw_choice);
    }

    fmt::print("Retrieved config value of {}! (alongside {} choices)\n", value, choice_count);

    this->disconnect();
    return GetConfigResponse{.value = value, .values = values};
}

ControllerResponse CameraController::capture_preview() {
    // Start of method, connect
    auto conn_response = this->connect();
    if (!conn_response.successful) {
        return ControllerResponse{false, conn_response.message};
    }

    this->disconnect();
    return ControllerResponse{};
}

ControllerResponse CameraController::capture_image() {
    // Start of method, connect
    auto conn_response = this->connect();
    if (!conn_response.successful) {
        return ControllerResponse{false, conn_response.message};
    }

    this->disconnect();
    return ControllerResponse{};
}


