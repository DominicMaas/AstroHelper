# Astro Helper

Astro helper is a Raspberry Pi peripheral and Flutter Mobile app that work together to expose functions from a DLSR to a mobile device over BLE.

It's aim is to help in astrophotograpy configuration and capture without needing to touch the camera (or be connected to any networks)

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
