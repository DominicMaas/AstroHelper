#include <iostream>
#include <gphoto2/gphoto2-camera.h>
#include "CameraController.h"

int main() {
    std::cout << "Hello, World! 12345" << std::endl;

    CameraController c;
    c.connect();

    Camera *camera;
    return gp_camera_new(&camera);
}
