#include <iostream>
#include <gphoto2/gphoto2-camera.h>

int main() {
    std::cout << "Hello, World!" << std::endl;

    Camera *camera;
    return gp_camera_new(&camera);
}
