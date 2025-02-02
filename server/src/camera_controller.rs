use crate::data::CameraConfig;
use std::time::Duration;

use log::info;

#[cfg(not(target_os = "windows"))]
use gphoto2::{widget::Widget, Camera, Context};

/// Abstracts away camera funtionality (as Windows does not support this,
/// allows for easy testing and development in windows)
pub struct CameraController {
    connected: bool,

    #[cfg(not(target_os = "windows"))]
    context: Option<Context>,

    #[cfg(not(target_os = "windows"))]
    camera: Option<Camera>,
}

impl CameraController {
    /// Create a new instance of the camera
    #[cfg(not(target_os = "windows"))]
    pub fn new() -> Self {
        CameraController {
            connected: false,
            context: None,
            camera: None,
        }
    }

    /// Create a new instance of the camera
    #[cfg(target_os = "windows")]
    pub fn new() -> Self {
        CameraController { connected: false }
    }

    /// Try and connect to the camera
    #[cfg(not(target_os = "windows"))]
    pub async fn try_connect(&mut self) -> bool {
        // Setup our context if no context has been setup
        if self.context.is_none() {
            match Context::new() {
                Ok(lc) => self.context = Some(lc),
                Err(e) => {
                    self.connected = false;

                    log::error!("Unable to get camera context: {}", e);
                    return false;
                }
            }
        }

        // Try auto connect to the camera
        if self.camera.is_none() {
            match self.context.as_mut().unwrap().autodetect_camera().await {
                Ok(cam) => {
                    self.connected = true;
                    self.camera = Some(cam);
                }
                Err(e) => {
                    self.connected = false;

                    log::error!("Unable to auto detect camera: {}", e);
                    return false;
                }
            }
        }

        // Keep things in sync
        if self.camera.is_some() {
            self.connected = true;

            return true;
        } else {
            self.connected = false;

            return false;
        }
    }

    /// Try and connect to the camera
    #[cfg(target_os = "windows")]
    pub async fn try_connect(&mut self) -> bool {
        if self.connected {
            return true;
        }

        tokio::time::sleep(Duration::from_secs(2)).await;

        self.connected = true;
        true
    }

    /// Attempts to get a config item from the camera
    #[cfg(not(target_os = "windows"))]
    pub async fn get_config(&mut self, option: &str) -> Option<CameraConfig> {
        info!("get_config({})", option);

        if !self.try_connect().await {
            log::error!("Camera is not connected!");
            return None;
        }

        if let Some(camera) = &self.camera {
            match camera.config_key::<Widget>(option).await {
                Ok(widget) => {
                    return match widget {
                        Widget::Group(_) => None,
                        Widget::Text(text_widget) => Some(CameraConfig {
                            id: option.to_string(),
                            value: text_widget.value(),
                            choices: vec![],
                            readonly: text_widget.readonly(),
                        }),
                        Widget::Range(range_widget) => Some(CameraConfig {
                            id: option.to_string(),
                            value: range_widget.value().to_string(),
                            choices: vec![],
                            readonly: range_widget.readonly(),
                        }),
                        Widget::Toggle(toggle_widget) => Some(CameraConfig {
                            id: option.to_string(),
                            value: match toggle_widget.toggled() {
                                Some(v) => v.to_string(),
                                None => false.to_string(),
                            },
                            choices: vec![],
                            readonly: toggle_widget.readonly(),
                        }),
                        Widget::Radio(radio_widget) => Some(CameraConfig {
                            id: option.to_string(),
                            value: radio_widget.choice(),
                            choices: radio_widget.choices_iter().collect(),
                            readonly: radio_widget.readonly(),
                        }),
                        Widget::Button(_) => None,
                        Widget::Date(date_widget) => Some(CameraConfig {
                            id: option.to_string(),
                            value: date_widget.timestamp().to_string(),
                            choices: vec![],
                            readonly: date_widget.readonly(),
                        }),
                    }
                }
                Err(e) => {
                    log::error!("Unable to get config: {}", e);
                    return None;
                }
            }
        }

        return None;
    }

    /// Attempts to get a config item from the camera
    #[cfg(target_os = "windows")]
    pub async fn get_config(&mut self, option: &str) -> Option<CameraConfig> {
        info!("get_config({})", option);

        if !self.try_connect().await {
            log::error!("Camera is not connected!");
            return None;
        }

        if option == "iso" {
            Some(CameraConfig {
                id: "iso".to_string(),
                value: "12800".to_string(),
                choices: [
                    "100", "200", "400", "800", "1600", "3200", "6400", "12800", "25600",
                ]
                .map(String::from)
                .to_vec(),
                readonly: false,
            })
        } else if option == "shutterspeed" {
            Some(CameraConfig {
                id: "shutterspeed".to_string(),
                value: "0.0050s".to_string(),
                choices: [
                    "0.0002s", "0.0003s", "0.0004s", "0.0005s", "0.0006s", "0.0008s", "0.0010s",
                    "0.0012s", "0.0015s", "0.0020s", "0.0025s", "0.0031s", "0.0040s", "0.0050s",
                    "0.0062s", "0.0080s", "0.0100s", "0.0125s", "0.0166s", "0.0200s", "0.0250s",
                    "0.0333s", "0.0400s", "0.0500s", "0.0666s", "0.0769s", "0.1000s", "0.1250s",
                    "0.1666s", "0.2000s", "0.2500s", "0.3333s", "0.4000s", "0.5000s", "0.6250s",
                    "0.7692s", "1.0000s", "1.3000s", "1.6000s", "2.0000s", "2.5000s", "3.0000s",
                    "4.0000s", "5.0000s", "6.0000s", "8.0000s", "10.0000s", "13.0000s", "15.0000s",
                    "20.0000s", "25.0000s", "30.0000s",
                ]
                .map(String::from)
                .to_vec(),
                readonly: false,
            })
        } else if option == "f-number" {
            Some(CameraConfig {
                id: "f-number".to_string(),
                value: "f/14".to_string(),
                choices: [
                    "f/4.5", "f/5", "f/5.6", "f/6.3", "f/7.1", "f/8", "f/9", "f/10", "f/11",
                    "f/13", "f/14", "f/16", "f/18", "f/20", "f/22",
                ]
                .map(String::from)
                .to_vec(),
                readonly: true,
            })
        } else {
            None
        }
    }

    /// Attempts to set a config item to the camera
    #[cfg(not(target_os = "windows"))]
    pub async fn set_config(&mut self, option: &str, value: &str) -> bool {
        info!("set_config({}, {})", option, value);

        if !self.try_connect().await {
            log::error!("Camera is not connected!");
            return false;
        }

        if let Some(camera) = &self.camera {
            match camera.config_key::<Widget>(option).await {
                Ok(widget) => {
                    return match widget {
                        Widget::Group(_) => {
                            log::error!("Unable to set config type: Group");
                            false
                        }
                        Widget::Text(text_widget) => match text_widget.set_value(value) {
                            Ok(_) => match camera.set_config(&text_widget).await {
                                Ok(_) => true,
                                Err(e) => {
                                    log::error!("Unable to set config: {}", e);
                                    false
                                }
                            },
                            Err(e) => {
                                log::error!("Unable to set config: {}", e);
                                false
                            }
                        },
                        Widget::Range(range_widget) => {
                            if let Ok(f) = value.parse::<f32>() {
                                range_widget.set_value(f);

                                return match camera.set_config(&range_widget).await {
                                    Ok(_) => true,
                                    Err(e) => {
                                        log::error!("Unable to set config: {}", e);
                                        false
                                    }
                                };
                            } else {
                                log::error!("Unable to parse {} to a f32.", value);
                                return false;
                            }
                        }
                        Widget::Toggle(toggle_widget) => {
                            let truth_value: bool = match value {
                                "true" => true,
                                "t" => true,
                                "1" => true,
                                "false" => false,
                                "f" => false,
                                "0" => false,
                                _ => false,
                            };

                            toggle_widget.set_toggled(truth_value);

                            return match camera.set_config(&toggle_widget).await {
                                Ok(_) => true,
                                Err(e) => {
                                    log::error!("Unable to set config: {}", e);
                                    false
                                }
                            };
                        }
                        Widget::Radio(radio_widget) => match radio_widget.set_choice(value) {
                            Ok(_) => match camera.set_config(&radio_widget).await {
                                Ok(_) => true,
                                Err(e) => {
                                    log::error!("Unable to set config: {}", e);
                                    false
                                }
                            },
                            Err(e) => {
                                log::error!("Unable to set config: {}", e);
                                false
                            }
                        },
                        Widget::Button(_) => {
                            log::error!("Unable to set config type: Button");
                            false
                        }
                        Widget::Date(_) => {
                            log::error!("Unable to set config type: Date");
                            false
                        }
                    };
                }
                Err(e) => {
                    log::error!("Unable to get config: {}", e);
                    return false;
                }
            }
        }

        false
    }

    /// Attempts to set a config item to the camera
    #[cfg(target_os = "windows")]
    pub async fn set_config(&mut self, option: &str, value: &str) -> bool {
        info!("set_config({}, {})", option, value);

        if !self.try_connect().await {
            log::error!("Camera is not connected!");
            return false;
        }

        true
    }

    /// Lists all of the possible configuration values for this camera
    #[cfg(not(target_os = "windows"))]
    pub async fn list_config(&mut self) -> Option<Vec<String>> {
        info!("list_config()");

        if !self.try_connect().await {
            log::error!("Camera is not connected!");
            return None;
        }

        match &self.camera {
            Some(camera) => match camera.config().await {
                Ok(c) => Some(c.children_iter().map(|f| f.name()).collect()),
                Err(e) => {
                    log::error!("Unable to get config: {}", e);
                    None
                }
            },
            _ => None,
        }
    }

    /// Lists all of the possible configuration values for this camera
    #[cfg(target_os = "windows")]
    pub async fn list_config(&mut self) -> Option<Vec<String>> {
        info!("list_config()");

        if !self.try_connect().await {
            log::error!("Camera is not connected!");
            return None;
        }

        Some(
            ["iso", "shutterspeed", "f-number"]
                .map(String::from)
                .to_vec(),
        )
    }

    #[cfg(not(target_os = "windows"))]
    pub async fn capture(&mut self) -> bool {
        info!("capture()");

        if !self.try_connect().await {
            log::error!("Camera is not connected!");
            return false;
        }

        if !self.set_config("capturetarget", "Memory card").await {
            log::error!("Canceling capture. Unable to set capturetarget!");
        }

        if !self.set_config("viewfinder", "0").await {
            log::error!("Canceling capture. Unable to set viewfinder!");
        }

        if !self.set_config("imagequality", "NEF (Raw)").await {
            log::error!("Canceling capture. Unable to set imagequality!");
        }

        if let Some(camera) = &self.camera {
            return match camera.capture_preview().await {
                Ok(f) => {
                    log::info!("Capture Success: {}", f.name());
                    true
                }
                Err(e) => {
                    log::error!("Capture Failed: {}", e);
                    false
                }
            };
        }

        false
    }

    #[cfg(target_os = "windows")]
    pub async fn capture(&mut self) -> bool {
        info!("capture()");

        if !self.try_connect().await {
            log::error!("Camera is not connected!");
            return false;
        }

        false
    }

    #[cfg(not(target_os = "windows"))]
    pub async fn capture_preview(&mut self) -> Option<Vec<u8>> {
        info!("capture_preview()");

        if !self.try_connect().await {
            log::error!("Camera is not connected!");
            return None;
        }

        None
    }

    #[cfg(target_os = "windows")]
    pub async fn capture_preview(&mut self) -> Option<Vec<u8>> {
        info!("capture_preview()");

        if !self.try_connect().await {
            log::error!("Camera is not connected!");
            return None;
        }

        None
    }

    /// Returns the current bettery level of the camera
    pub async fn get_battery_level(&mut self) -> Option<u8> {
        info!("get_battery_level()");

        if !self.try_connect().await {
            log::error!("Camera is not connected!");
            return None;
        }

        Some(80)
    }
}
