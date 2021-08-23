//
// Created by Dominic Maas on 23/08/21.
//

#include "CameraController.h"

bool CameraController::connect() {
    int res = gp_camera_new(&_camera);
    printf("gp_camera_new returned %i", res);

    if (res == 0) {
        printf("gp_camera_new returned 0");
        return false;
    }

    GPContext *context = gp_context_new();

    CameraText camText;
    gp_camera_get_about(_camera, &camText, context);

    printf("Camera Text: %s", camText.text);
    return true;
}
