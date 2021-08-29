#include "camera_controller.h"

ControllerResponse CameraController::connect() {   
    // Required context and camera init
    if (this->_context != nullptr) {      
        this->_context = gp_context_new();
    }
    
    if (this->_camera != nullptr) {
        // Attempt to init the camera
        gp_camera_new(&this->_camera);
        int init_res = gp_camera_init(this->_camera, this->_context);
        if (init_res != 0) {
            this->_camera = nullptr;
            
            auto message = fmt::format("Unable to connect to camera. Result: {}", gp_result_as_string(init_res));

            fmt::print("{}\n", message);
            return ControllerResponse{false, message};
        }
    }
    

    return ControllerResponse{};
}

ControllerResponse CameraController::disconnect() {
    if (this->_camera != nullptr) {
        gp_camera_exit(this->_camera, this->_context);
    }

    return ControllerResponse{};
}

ControllerResponse CameraController::set_config_item(const std::string &name, const std::string &value) {
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
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to get camera config. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);
        return ControllerResponse{false, message};
    }

    // Get the widget
    CameraWidget *widget;
    res = gp_widget_get_child_by_name(config, name.c_str(), &widget);
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to get specified camera config ({}). Result: {}", name,
                                   gp_result_as_string(res));

        fmt::print("{}\n", message);
        return ControllerResponse{false, message};
    }

    // Extract the type
    CameraWidgetType type;
    gp_widget_get_type(widget, &type);

    // Extract if readonly
    int read_only;
    gp_widget_get_readonly(widget, &read_only);

    // Cannot modify readonly
    if (read_only == 1) {
        auto message = fmt::format("Unable to get specified camera config ({}). Result: This config is readonly!",
                                   name);

        fmt::print("{}\n", message);

        return ControllerResponse{false, message};
    }

    // Attempt to set the value
    switch (type) {
        case CameraWidgetType::GP_WIDGET_RANGE: // FLOAT
        {
            float f = std::stof(value);
            res = gp_widget_set_value(widget, &f);
            break;
        }

        case CameraWidgetType::GP_WIDGET_TOGGLE: // INT
        case CameraWidgetType::GP_WIDGET_DATE:   // INT
        {
            int i = std::stoi(value);
            res = gp_widget_set_value(widget, &i);
            break;
        }

        default: // STRING
            res = gp_widget_set_value(widget, value.c_str());
            break;
    }

    // Handle set response
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to set camera config ({}) value. Result: {}", name,
                                   gp_result_as_string(res));

        fmt::print("{}\n", message);
        return ControllerResponse{false, message};
    }

    // Attempt to set the config
    res = gp_camera_set_config(this->_camera, config, this->_context);
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to set camera config. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);
        return ControllerResponse{false, message};
    }

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
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to get camera config. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);
        return GetConfigResponse{false, message};
    }

    // Get the widget
    CameraWidget *widget;
    res = gp_widget_get_child_by_name(config, name.c_str(), &widget);
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to get specified camera config ({}). Result: {}", name,
                                   gp_result_as_string(res));

        fmt::print("{}\n", message);
        return GetConfigResponse{false, message};
    }

    // Where we will eventually store the value
    std::string value;

    // Extract the type
    CameraWidgetType type;
    gp_widget_get_type(widget, &type);

    // Extract if readonly
    int read_only;
    gp_widget_get_readonly(widget, &read_only);

    // Extract depending on the type
    switch (type) {
        case CameraWidgetType::GP_WIDGET_RANGE: // FLOAT
            float raw_float;
            res = gp_widget_get_value(widget, &raw_float);
            if (res != 0) {
                break;
            }

            value = std::to_string(raw_float);
            break;

        case CameraWidgetType::GP_WIDGET_TOGGLE: // INT
        case CameraWidgetType::GP_WIDGET_DATE:   // INT
            int raw_int;
            res = gp_widget_get_value(widget, &raw_int);
            if (res != 0) {
                break;
            }

            value = std::to_string(raw_int);
            break;

        default: // STRING
            const char *raw_str;
            res = gp_widget_get_value(widget, &raw_str);
            if (res != 0) {
                break;
            }

            value = raw_str;
            break;
    }

    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to get specified camera config value ({}). Result: {}", name,
                                   gp_result_as_string(res));

        fmt::print("{}\n", message);
        return GetConfigResponse{false, message};
    }

    auto values = std::vector<std::string>();
    auto choice_count = gp_widget_count_choices(widget);

    // Assume choices are strings
    if (choice_count > 0) {
        for (int i = 0; i < choice_count; i++) {
            const char *raw_choice;
            gp_widget_get_choice(widget, i, &raw_choice);

            values.emplace_back(raw_choice);
        }
    }

    return GetConfigResponse{.value = value, .values = values, .read_only = read_only == 1};
}

CameraPreviewResponse CameraController::capture_preview() {
    // Start of method, connect
    auto conn_response = this->connect();
    if (!conn_response.successful) {
        return CameraPreviewResponse{false, conn_response.message};
    }

    // Keep track of return codes
    int res;

    // Get the config
    CameraWidget *config;
    res = gp_camera_get_config(this->_camera, &config, this->_context);
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to get camera config. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);
        return CameraPreviewResponse{false, message};
    }

    // Get the widgets
    auto capture_target_widget_res = CameraController::get_config_internal(config, "capturetarget");
    if (!capture_target_widget_res.successful) {
        this->_camera = nullptr;
        return CameraPreviewResponse{capture_target_widget_res.successful, capture_target_widget_res.message};
    }

    auto view_finder_widget_res = CameraController::get_config_internal(config, "viewfinder");
    if (!view_finder_widget_res.successful) {
        this->_camera = nullptr;
        return CameraPreviewResponse{view_finder_widget_res.successful, view_finder_widget_res.message};
    }

    auto image_quality_widget_res = CameraController::get_config_internal(config, "imagequality");
    if (!image_quality_widget_res.successful) {
        this->_camera = nullptr;
        return CameraPreviewResponse{image_quality_widget_res.successful, image_quality_widget_res.message};
    }

    // Set the Capture Target to 'Internal RAM'
    res = gp_widget_set_value(capture_target_widget_res.widget, "Internal RAM");
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Could not set the capture target to Internal RAM");
        
        fmt::print("{}\n", message);
        return CameraPreviewResponse{false, message};
    }

    // Set the view finder to 0
    int val = 0;
    res = gp_widget_set_value(view_finder_widget_res.widget, &val);
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Could not set the view finder to 0");
        
        fmt::print("{}\n", message);
        return CameraPreviewResponse{false, message};
    }

    // Set the image format to 'JPEG Normal'
    res = gp_widget_set_value(image_quality_widget_res.widget, "JPEG Normal");
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Could not set the image quality to JPEG Normal");
        
        fmt::print("{}\n", message);
        return CameraPreviewResponse{false, message};
    }

    // Attempt to set the config
    res = gp_camera_set_config(this->_camera, config, this->_context);
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to set camera config. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);
        return CameraPreviewResponse{false, message};
    }

    CameraFilePath file_path;
    res = gp_camera_capture(this->_camera, CameraCaptureType::GP_CAPTURE_IMAGE, &file_path, this->_context);
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to capture image (GP_CAPTURE_IMAGE). Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);
        return CameraPreviewResponse{false, message};
    }

    fmt::print("Saved file to {} / {} !\n", file_path.folder, file_path.name);

    CameraFile *file;
    gp_file_new(&file);

    res = gp_camera_file_get(this->_camera, file_path.folder, file_path.name, CameraFileType::GP_FILE_TYPE_NORMAL, file,
                             this->_context);
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to get camera file. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);
        return CameraPreviewResponse{false, message};
    }

    const char *raw_data;
    unsigned long int size;

    res = gp_file_get_data_and_size(file, &raw_data, &size);
    if (res != 0) {
        this->_camera = nullptr;
        
        auto message = fmt::format("Unable to get camera file data and size. Result: {}", gp_result_as_string(res));

        fmt::print("{}\n", message);

        this->disconnect();
        return CameraPreviewResponse{false, message};
    }
    
    std::vector<char> data(raw_data, raw_data + size);
    return CameraPreviewResponse{true, "", size, data};
}

ControllerResponse CameraController::capture_image() {
    // Start of method, connect
    auto conn_response = this->connect();
    if (!conn_response.successful) {
        return ControllerResponse{false, conn_response.message};
    }

    return ControllerResponse{};
}

ConfigResponse CameraController::get_config_internal(CameraWidget *config, const std::string &name) {
    CameraWidget *widget;
    auto res = gp_widget_get_child_by_name(config, name.c_str(), &widget);
    if (res != 0) {
        auto message = fmt::format("There are no widgets with the name of '{}'! Result: {}",
                                   name, gp_result_as_string(res));

        fmt::print("{}\n", message);

        return ConfigResponse{false, message};
    }

    return ConfigResponse{true, "", widget};
}


