# Astro Helper

Astro helper is a Raspberry Pi peripheral and Flutter Mobile app that work together to expose functions from a DLSR to a mobile device over BLE.

It's aim is to help in astrophotograpy configuration and capture without needing to touch the camera (or be connected to any networks)

![image](https://github.com/user-attachments/assets/f3b09351-1ff8-4bfd-8cc9-a1fd81f6ca70)

![image](https://github.com/user-attachments/assets/be29e7be-18ba-410b-b544-a46a2c974725)

```
[2025-02-02T23:51:36Z DEBUG astro] WriteRequest: PeripheralRequest { client: "BC:F1:71:AC:61:E1", service: 6c7028e2-dc4a-11ef-9134-75e88a34574d, characteristic: 87bdecc4-dc4a-11ef-ab7f-b7e88a34574d } Offset: 0 Value: [105, 115, 111]
[2025-02-02T23:51:36Z INFO  astro::camera_controller] get_config(iso)
[2025-02-02T23:51:37Z DEBUG astro] Config Data is Some(CameraConfig { id: "iso", value: "3200", choices: ["100", "200", "400", "800", "1600", "3200", "6400", "12800", "25600"], readonly: false })
[2025-02-02T23:51:37Z DEBUG astro::data] Uncompressed Data Size: 142
[2025-02-02T23:51:37Z DEBUG astro::data] Compressed Data Size: 89
[2025-02-02T23:51:42Z DEBUG astro] WriteRequest: PeripheralRequest { client: "BC:F1:71:AC:61:E1", service: 6c7028e2-dc4a-11ef-9134-75e88a34574d, characteristic: 87bdecc4-dc4a-11ef-ab7f-b7e88a34574d } Offset: 0 Value: [102, 45, 110, 117, 109, 98, 101, 114]
[2025-02-02T23:51:42Z INFO  astro::camera_controller] get_config(f-number)
[2025-02-02T23:51:42Z DEBUG astro] Config Data is Some(CameraConfig { id: "f-number", value: "f/6.3", choices: ["f/4.8", "f/5", "f/5.6", "f/6.3", "f/7.1", "f/8", "f/9", "f/10", "f/11", "f/13", "f/14", "f/16", "f/18", "f/20", "f/22", "f/25"], readonly: true })
[2025-02-02T23:51:42Z DEBUG astro::data] Uncompressed Data Size: 235
[2025-02-02T23:51:42Z DEBUG astro::data] Compressed Data Size: 129
[2025-02-02T23:51:51Z DEBUG astro] WriteRequest: PeripheralRequest { client: "BC:F1:71:AC:61:E1", service: 6c7028e2-dc4a-11ef-9134-75e88a34574d, characteristic: 87bdecc4-dc4a-11ef-ab7f-b7e88a34574d } Offset: 0 Value: [115, 104, 117, 116, 116, 101, 114, 115, 112, 101, 101, 100]
[2025-02-02T23:51:51Z INFO  astro::camera_controller] get_config(shutterspeed)
[2025-02-02T23:51:51Z DEBUG astro] Config Data is Some(CameraConfig { id: "shutterspeed", value: "0.0050s", choices: ["0.0002s", "0.0003s", "0.0004s", "0.0005s", "0.0006s", "0.0008s", "0.0010s", "0.0012s", "0.0015s", "0.0020s", "0.0025s", "0.0031s", "0.0040s", "0.0050s", "0.0062s", "0.0080s", "0.0100s", "0.0125s", "0.0166s", "0.0200s", "0.0250s", "0.0333s", "0.0400s", "0.0500s", "0.0666s", "0.0769s", "0.1000s", "0.1250s", "0.1666s", "0.2000s", "0.2500s", "0.3333s", "0.4000s", "0.5000s", "0.6250s", "0.7692s", "1.0000s", "1.3000s", "1.6000s", "2.0000s", "2.5000s", "3.0000s", "4.0000s", "5.0000s", "6.0000s", "8.0000s", "10.0000s", "13.0000s", "15.0000s", "20.0000s", "25.0000s", "30.0000s"], readonly: false })
[2025-02-02T23:51:51Z DEBUG astro::data] Uncompressed Data Size: 834
[2025-02-02T23:51:51Z DEBUG astro::data] Compressed Data Size: 292
[2025-02-02T23:52:10Z DEBUG astro] WriteRequest: PeripheralRequest { client: "BC:F1:71:AC:61:E1", service: 6c7028e2-dc4a-11ef-9134-75e88a34574d, characteristic: 994e9452-dc4a-11ef-b90e-f0e88a34574d } Offset: 0 Value: [115, 104, 117, 116, 116, 101, 114, 115, 112, 101, 101, 100, 61, 48, 46, 48, 51, 51, 51, 115]
[2025-02-02T23:52:10Z INFO  astro::camera_controller] set_config(shutterspeed, 0.0333s)
[2025-02-02T23:52:15Z DEBUG astro] WriteRequest: PeripheralRequest { client: "BC:F1:71:AC:61:E1", service: 6c7028e2-dc4a-11ef-9134-75e88a34574d, characteristic: 114fc821-6a6e-4e81-bd05-1dd5ab7a679b } Offset: 0 Value: [118]
[2025-02-02T23:52:15Z INFO  astro::camera_controller] capture()
[2025-02-02T23:52:15Z INFO  astro::camera_controller] set_config(capturetarget, Memory card)
[2025-02-02T23:52:15Z INFO  astro::camera_controller] set_config(viewfinder, 0)
[2025-02-02T23:52:15Z INFO  astro::camera_controller] set_config(imagequality, NEF (Raw))
[2025-02-02T23:52:16Z INFO  astro::camera_controller] Capture Success: capture_preview.jpg
```

## Building

### Application (via Windows)

- `cross build`
- `scp .\target\aarch64-unknown-linux-gnu\debug\astro_server astro@192.168.1.88:/home/astro`

### Library

- `cargo build --release`

## BLE Service

The Astro Helper peripheral exposes a few services.

### Battery Service
**UUID:** 0x0000180F_0000_1000_8000_00805f9b34fb

### Configuration Service

**UUID:** 0x6C7028E2_DC4A_11EF_9134_75E88A34574D
