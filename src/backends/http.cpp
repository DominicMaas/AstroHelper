#include "http.h"

HTTP::HTTP(CameraController *controller) {
    this->_controller = controller;

    // Setup HTTP Bindings

    this->_server.Get("/get-config-item/", [this](const httplib::Request &req, httplib::Response &res) {

        auto response = this->_controller->get_config_item("/invalid");

        res.set_content("Hello World!", "text/plain");
    });

    this->_server.Get("/set-config-item/", [this](const httplib::Request &req, httplib::Response &res) {

        auto response = this->_controller->set_config_item("/invalid", "test");

        res.set_content("Hello World!", "text/plain");
    });

    this->_server.Get("/capture-preview", [this](const httplib::Request &req, httplib::Response &res) {

        auto response = this->_controller->capture_preview();

        res.set_content("Hello World!", "text/plain");
    });

    this->_server.Get("/capture-image", [this](const httplib::Request &req, httplib::Response &res) {

        auto response = this->_controller->capture_image();

        res.set_content("Hello World!", "text/plain");
    });
}

void HTTP::listen() {
    this->_server.listen("0.0.0.0", 8080);
}
