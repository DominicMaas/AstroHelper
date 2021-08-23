//
// Created by Dominic Maas on 23/08/21.
//

#include "CameraController.h"

bool CameraController::connect() {
    int res = gp_camera_new(&_camera);
    std::cout << "gp_camera_new returned " << res << std::endl;

    if (res == 0) {
        std::cout << "Failed to connect to camera!" << std::endl;
        return false;
    }

    GPContext *context = gp_context_new();

    CameraText camText;
    gp_camera_get_about(_camera, &camText, context);

    std::cout << "Camera Text: " << camText.text << std::endl;
    return true;
}
