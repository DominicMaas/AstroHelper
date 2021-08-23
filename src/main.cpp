#include <iostream>
#include <gphoto2/gphoto2-camera.h>
#include "CameraController.h"

#include <httplib.h>

int main() {
    std::cout << "Hello, World! 12345" << std::endl;

    CameraController c;
    c.connect();

    c.get_config_item("/main/capturesettings/applicationmode");

    return 0;
}
