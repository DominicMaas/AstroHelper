mod camera_controller;
mod data;

use ble_peripheral_rust::{
    gatt::{
        characteristic::Characteristic,
        peripheral_event::{
            PeripheralEvent, ReadRequestResponse, RequestResponse, WriteRequestResponse,
        },
        properties::{AttributePermission, CharacteristicProperty},
        service::Service,
    },
    Peripheral, PeripheralImpl,
};
use camera_controller::CameraController;
use data::{encode_data, AstroData};
use log::info;
use tokio::sync::mpsc;
use uuid::Uuid;

static BATTERY_ID: Uuid = Uuid::from_u128(0x0000180F_0000_1000_8000_00805f9b34fb);
static BATTERY_LEVEL_ID: Uuid = Uuid::from_u128(0x00002A19_0000_1000_8000_00805f9b34fb);

static CONFIG_ID: Uuid = Uuid::from_u128(0x6C7028E2DC4A11EF913475E88A34574D);
static CONFIG_READ_ID: Uuid = Uuid::from_u128(0x87BDECC4DC4A11EFAB7FB7E88A34574D);
static CONFIG_WRITE_ID: Uuid = Uuid::from_u128(0x994E9452DC4A11EFB90EF0E88A34574D);
static CONFIG_LIST_ID: Uuid = Uuid::from_u128(0x9C39AB84DC4A11EF8C45FAE88A34574D);
static CONFIG_CAPTURE_ID: Uuid = Uuid::from_u128(0x114fc821_6a6e_4e81_bd05_1dd5ab7a679b);

#[tokio::main]
async fn main() {
    env_logger::init();

    info!("Welcome to astro_helper! Starting BLE stack...");

    // A standard bluetooth battery service to forward camera battery
    let battery_service = Service {
        uuid: BATTERY_ID,
        primary: true,
        characteristics: vec![Characteristic {
            uuid: BATTERY_LEVEL_ID,
            properties: vec![CharacteristicProperty::Read],
            permissions: vec![AttributePermission::Readable],
            ..Default::default()
        }],
    };

    let config_service = Service {
        uuid: CONFIG_ID,
        primary: true,
        characteristics: vec![
            // ReadConfigItem
            Characteristic {
                uuid: CONFIG_READ_ID,
                properties: vec![
                    CharacteristicProperty::Write,
                    CharacteristicProperty::Notify,
                ],
                permissions: vec![AttributePermission::Writeable],
                ..Default::default()
            },
            // WriteConfigItem
            Characteristic {
                uuid: CONFIG_WRITE_ID,
                properties: vec![CharacteristicProperty::Write],
                permissions: vec![AttributePermission::Writeable],
                ..Default::default()
            },
            // ListConfigItems
            Characteristic {
                uuid: CONFIG_LIST_ID,
                properties: vec![CharacteristicProperty::Read, CharacteristicProperty::Notify],
                permissions: vec![AttributePermission::Readable],
                ..Default::default()
            },
            // CaptureImage
            Characteristic {
                uuid: CONFIG_CAPTURE_ID,
                properties: vec![CharacteristicProperty::Write],
                permissions: vec![AttributePermission::Writeable],
                ..Default::default()
            },
        ],
    };

    let (sender_tx, mut receiver_rx) = mpsc::channel::<PeripheralEvent>(256);

    let mut peripheral = Peripheral::new(sender_tx).await.unwrap();

    let mut camera = CameraController::new();

    info!("Waiting for BLE peripheral to power on...");

    // Ensure the peripheral is powered on
    while !peripheral.is_powered().await.unwrap() {}

    // ------------ Add Services ------------ //

    if let Err(err) = peripheral.add_service(&battery_service).await {
        log::error!("Error adding battery service: {}", err);
        return;
    }

    if let Err(err) = peripheral.add_service(&config_service).await {
        log::error!("Error adding config service: {}", err);
        return;
    }

    // -------------------------------------- //

    if let Err(err) = peripheral
        .start_advertising("Astro Helper", &[battery_service.uuid, config_service.uuid])
        .await
    {
        log::error!("Error starting advertising: {}", err);
        return;
    }

    // Handle Updates
    while let Some(event) = receiver_rx.recv().await {
        handle_updates(event, &mut camera, &mut peripheral).await;
    }
}

/// Listen to all updates and respond if require
async fn handle_updates(
    update: PeripheralEvent,
    camera: &mut CameraController,
    peripheral: &mut Peripheral,
) {
    match update {
        PeripheralEvent::StateUpdate { is_powered } => {
            log::info!("PowerOn: {is_powered:?}")
        }
        PeripheralEvent::CharacteristicSubscriptionUpdate {
            request,
            subscribed,
        } => {
            log::info!("CharacteristicSubscriptionUpdate: Subscribed {subscribed} {request:?}")
        }
        PeripheralEvent::ReadRequest {
            request,
            offset,
            responder,
        } => {
            log::debug!("ReadRequest: {request:?} Offset: {offset:?}");

            if request.characteristic == BATTERY_LEVEL_ID {
                if let Some(battery) = camera.get_battery_level().await {
                    responder
                        .send(ReadRequestResponse {
                            value: [battery].into(),
                            response: RequestResponse::Success,
                        })
                        .unwrap();
                } else {
                    responder
                        .send(ReadRequestResponse {
                            value: [0].into(),
                            response: RequestResponse::RequestNotSupported,
                        })
                        .unwrap();
                }
            } else if request.characteristic == CONFIG_LIST_ID {
                if let Some(config_list) = camera.list_config().await {
                    log::debug!("Config list is {:?}", config_list);

                    let encoded_data = bincode::serialize(&config_list).unwrap();
                    log::debug!("encoded_data length is {}", encoded_data.len());

                    peripheral
                        .update_characteristic(CONFIG_LIST_ID, encoded_data)
                        .await
                        .unwrap();

                    responder
                        .send(ReadRequestResponse {
                            value: config_list.join(",").into(),
                            response: RequestResponse::Success,
                        })
                        .unwrap();
                }
            }
        }
        PeripheralEvent::WriteRequest {
            request,
            offset,
            value,
            responder,
        } => {
            log::debug!("WriteRequest: {request:?} Offset: {offset:?} Value: {value:?}");

            // Reading Config ID
            if request.characteristic == CONFIG_READ_ID {
                let config_value = match String::from_utf8(value) {
                    Ok(str) => camera.get_config(&str).await,
                    Err(_) => None,
                };

                if config_value.is_some() {
                    log::debug!("Config Data is {:?}", config_value);
                    let encoded_data = encode_data(AstroData::CameraConfig(config_value.unwrap()));

                    peripheral
                        .update_characteristic(CONFIG_READ_ID, encoded_data)
                        .await
                        .unwrap();

                    responder
                        .send(WriteRequestResponse {
                            response: RequestResponse::Success,
                        })
                        .unwrap()
                } else {
                    responder
                        .send(WriteRequestResponse {
                            response: RequestResponse::UnlikelyError,
                        })
                        .unwrap()
                }
            } else
            // Writing to Config ID. Format X=Y
            if request.characteristic == CONFIG_WRITE_ID {
                let config_was_set = match String::from_utf8(value) {
                    Ok(str) => match str.split_once('=') {
                        Some((op, val)) => camera.set_config(op, val).await,
                        None => false,
                    },
                    Err(_) => false,
                };

                if config_was_set {
                    responder
                        .send(WriteRequestResponse {
                            response: RequestResponse::Success,
                        })
                        .unwrap()
                } else {
                    responder
                        .send(WriteRequestResponse {
                            response: RequestResponse::UnlikelyError,
                        })
                        .unwrap()
                }
            } else
            // Capturing image
            if request.characteristic == CONFIG_CAPTURE_ID {
                camera.capture().await;

                responder
                    .send(WriteRequestResponse {
                        response: RequestResponse::Success,
                    })
                    .unwrap()
            }
        }
    }
}
