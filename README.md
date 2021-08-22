# Introduction

Exposes a gphoto2 connection over network via BLE or HTTP. Useful for running on a raspberry pi to control 
cameras remotely via other devices (such as a mobile phone)

# Requirements

This project has been developed on macOS and Linux. Windows is not supported. BLE is only supported on Linux.

## libgphoto2

_Access camera functionality over USB connection_

- **macOS:** `brew install libgphoto2`
- **Linux:** `apt-get install libgphoto2-dev`

## boost 1.76

_Required for Boost Beast (HTTP Server)_

- **macOS:** `brew install boost`
- **Linux:** `apt-get install boost`

# Structure 

The ganymede server supports operating in two modes, as a bluetooth peripheral (via BLE) and as an HTTP Server. 
Camera functionality is abstracted behind the `CameraController` class. Methods take in simple strings as input data, 
and expose output data in form of protobuf objects.

The BLE Server and HTTP Server then abstract over this `CameraController` class exposing the methods as endpoints that can 
be accessed remotely.