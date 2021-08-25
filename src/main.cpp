#include <iostream>
#include "camera_controller.h"
#include "backends/http.h"

int main() {
    // Controller to access camera
    auto controller = new CameraController();
    controller->connect();

    // HTTP Server
    HTTP http(controller);
    http.listen();

    controller->disconnect();
    delete controller;
    return 0;
}
