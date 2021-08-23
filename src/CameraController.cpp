//
// Created by Dominic Maas on 23/08/21.
//

#include "CameraController.h"

bool CameraController::connect() {
    // TODO: What happens if connect is called twice? Clean up old resources

    // Required context and camera init
    this->_context = gp_context_new();

    // Attempt to init the camera
    gp_camera_new(&this->_camera);
    int init_res = gp_camera_init(this->_camera, this->_context);
    if (init_res != 0) {
        std::cout << "Unable to connect to camera: " << gp_result_as_string(init_res) << std::endl;
        return false;
    }

    CameraText camText;
    gp_camera_get_about(this->_camera, &camText, this->_context);

    std::cout << "Camera Text: " << camText.text << std::endl;
    return true;
}

bool CameraController::disconnect() {
    if (this->_camera != nullptr) {
        gp_camera_exit(this->_camera, this->_context);
    }


    return false;
}
