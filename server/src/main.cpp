#include <iostream>
#include "camera_controller.h"
#include "backends/http.h"

int main() {
    // Controller to access camera
    auto controller = new CameraController();

    // HTTP Server
    HTTP http(controller);
    http.listen();

    // Finish up
    delete controller;
    return 0;
}
