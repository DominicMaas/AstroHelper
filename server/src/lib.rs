mod data;

use std::ffi::CString;
use std::os::raw::c_char;
use std::ptr;

use data::{decode_data, AstroData};

#[repr(C)]
pub struct CameraConfigC {
    id: *mut c_char,
    value: *mut c_char,
    choices: *mut *mut c_char,
    choices_len: usize,
    readonly: bool,
}

// Helper function to allocate C strings
fn to_c_string(s: &str) -> *mut c_char {
    CString::new(s).unwrap().into_raw()
}

#[no_mangle]
pub extern "C" fn decode_camera_config(data: *const u8, len: usize) -> *mut CameraConfigC {
    if data.is_null() || len == 0 {
        return ptr::null_mut();
    }

    let slice = unsafe { std::slice::from_raw_parts(data, len) };
    match decode_data(slice) {
        Some(data) => match data {
            AstroData::CameraConfig(config) => {
                let mut choices_c: Vec<*mut c_char> =
                    config.choices.iter().map(|s| to_c_string(s)).collect();

                // Allocate choices array on heap
                let choices_ptr = choices_c.as_mut_ptr();
                std::mem::forget(choices_c);

                return Box::into_raw(Box::new(CameraConfigC {
                    id: to_c_string(&config.id),
                    value: to_c_string(&config.value),
                    choices: choices_ptr,
                    choices_len: config.choices.len(),
                    readonly: config.readonly,
                }));
            }
        },
        None => ptr::null_mut(),
    }
}

// Function to free the allocated struct
#[no_mangle]
pub extern "C" fn free_camera_config(ptr: *mut CameraConfigC) {
    if ptr.is_null() {
        return;
    }

    unsafe {
        let config = Box::from_raw(ptr);

        // Free the id string
        if !config.id.is_null() {
            drop(CString::from_raw(config.id));
        }

        // Free the value string
        if !config.value.is_null() {
            drop(CString::from_raw(config.value));
        }

        // Free choices array safely
        if !config.choices.is_null() {
            let choices_slice = std::slice::from_raw_parts(config.choices, config.choices_len);
            for &choice in choices_slice {
                if !choice.is_null() {
                    drop(CString::from_raw(choice));
                }
            }
            drop(Box::from_raw(config.choices));
        }
    }
}

#[cfg(test)]
mod tests {
    use std::ffi::CStr;

    use crate::data::encode_data;
    use crate::data::CameraConfig;

    use super::*;

    fn init() {
        let _ = env_logger::builder().is_test(true).try_init();
    }

    #[test]
    fn it_works() {
        init();

        let start_config = CameraConfig {
            id: "iso".to_string(),
            value: "12800".to_string(),
            choices: [
                "100", "200", "400", "800", "1600", "3200", "6400", "12800", "25600",
            ]
            .map(String::from)
            .to_vec(),
            readonly: false,
        };

        let encoded_data = encode_data(AstroData::CameraConfig(start_config.clone()));
        let config_ptr = decode_camera_config(encoded_data.as_ptr(), encoded_data.len());

        if config_ptr.is_null() {
            assert!(false);
        }

        // Convert the pointer back to a Rust reference
        let config = unsafe { &*config_ptr };

        assert_eq!(start_config.id, unsafe {
            CStr::from_ptr(config.id).to_string_lossy()
        });

        assert_eq!(start_config.value, unsafe {
            CStr::from_ptr(config.value).to_string_lossy()
        });

        assert_eq!(start_config.readonly, config.readonly);

        let choices_slice =
            unsafe { std::slice::from_raw_parts(config.choices, config.choices_len) };

        let choices: Vec<String> = choices_slice
            .iter()
            .map(|&ptr| unsafe { CStr::from_ptr(ptr).to_string_lossy().into_owned() })
            .collect();

        assert_eq!(start_config.choices, choices);

        // Free the allocated memory
        free_camera_config(config_ptr);
    }
}
