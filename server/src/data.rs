use lz4_flex::{compress_prepend_size, decompress_size_prepended};
use serde::{Deserialize, Serialize};

#[derive(Serialize, Deserialize, PartialEq, Debug, Clone)]
pub struct CameraConfig {
    pub id: String,
    pub value: String,
    pub choices: Vec<String>,
    pub readonly: bool,
}

#[derive(Serialize, Deserialize, PartialEq, Debug, Clone)]
pub enum AstroData {
    CameraConfig(CameraConfig),
}

pub fn encode_data(data: AstroData) -> Vec<u8> {
    let uncompressed = bincode::serialize(&data).unwrap();
    log::debug!("Uncompressed Data Size: {}", uncompressed.len());

    let compressed = compress_prepend_size(&uncompressed);
    log::debug!("Compressed Data Size: {}", compressed.len());

    return compressed;
}

pub fn decode_data(input: &[u8]) -> Option<AstroData> {
    match decompress_size_prepended(input) {
        Ok(uncompressed) => match bincode::deserialize::<AstroData>(&uncompressed) {
            Ok(data) => Some(data),
            Err(_) => {
                log::error!("Unable to deserialize!");
                None
            }
        },
        Err(_) => {
            log::error!("Unable to decompress!");
            None
        }
    }
}
