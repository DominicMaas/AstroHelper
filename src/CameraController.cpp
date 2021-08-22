//
// Created by Dominic Maas on 23/08/21.
//

#include "CameraController.h"

bool CameraController::connect() {
    auto res = gp_camera_new(&_camera);
    if (res == 0) {
        return false;
    }

    return true;
}
