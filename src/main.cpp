#include <iostream>
#include <gphoto2/gphoto2-camera.h>
#include "CameraController.h"

#include <httplib.h>

int main() {
    std::cout << "Hello, World! 12345" << std::endl;

    CameraController c;
    c.connect();

    return 0;
}
