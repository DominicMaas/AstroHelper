#include "http.h"

HTTP::HTTP(CameraController *controller) {
    this->_controller = controller;

    // Setup HTTP Bindings
    this->_server.Get("/get-config-item/(.+)", [this](const httplib::Request &req, httplib::Response &res) {
        // Extract name and try extract config
        auto configName = req.matches[1].str();
        auto response = this->_controller->get_config_item(configName);

        if (response.successful) {
            nlohmann::json j;
            j["value"] = response.value;
            j["choices"] = response.values;
            j["read_only"] = response.read_only;

            res.set_content(j.dump(), "application/json");
        } else {
            res.status = 500;
            res.set_content(response.message, "text/plain");
        }
    });

    this->_server.Get("/set-config-item/(.+)", [this](const httplib::Request &req, httplib::Response &res) {
        // Extract name and try extract config
        auto configName = req.matches[1].str();
        auto response = this->_controller->set_config_item(configName, req.body);

        if (response.successful) {
            res.set_content("Success!", "text/plain");
        } else {
            res.status = 500;
            res.set_content(response.message, "text/plain");
        }
    });

    this->_server.Get("/capture-preview", [this](const httplib::Request &req, httplib::Response &res) {
        auto response = this->_controller->capture_preview();
        if (response.successful) {
            // TODO
            res.set_content("Success!", "text/plain");
        } else {
            res.status = 500;
            res.set_content(response.message, "text/plain");
        }
    });

    this->_server.Get("/capture-image", [this](const httplib::Request &req, httplib::Response &res) {
        auto response = this->_controller->capture_image();
        if (response.successful) {
            // TODO
            res.set_content("Success!", "text/plain");
        } else {
            res.status = 500;
            res.set_content(response.message, "text/plain");
        }
    });
}

void HTTP::listen() {
    this->_server.listen("0.0.0.0", 8080);
}
