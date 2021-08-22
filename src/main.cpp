#include <iostream>
#include <gphoto2/gphoto2-camera.h>

#include <boost/beast.hpp>

int main() {
    std::cout << "Hello, World! 12345" << std::endl;

    Camera *camera;
    return gp_camera_new(&camera);
}
