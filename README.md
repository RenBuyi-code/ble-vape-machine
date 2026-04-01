# BLE Vape Machine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

[中文](README_CN.md) | English

---

## Overview

This is a BLE (Bluetooth Low Energy) based vape machine control system firmware, designed for BK3432 BLE SoC platform. It provides complete hardware control capabilities including motor control, battery management, charging control, and RGB LED effects.

## Features

- **BLE 4.2/5.0 Connectivity**: Based on Beken BK3432 BLE SoC
- **Motor Control**: Supports up to 10 channels (1-5 for rotary machines, 1-10 for grid machines)
- **Battery Management**: Real-time voltage monitoring and low battery protection
- **Charging Control**: Hardware charging enable/disable control
- **RGB LED Effects**: WS2812 RGB LED strip support with configurable colors
- **AT Command Interface**: UART-based AT command interface for device configuration
- **OTA Support**: Over-the-air firmware update capability
- **Password Protection**: Configurable access control with password authentication

## Hardware Platform

- **MCU**: Beken BK3432 (ARM Cortex-M4)
- **Development Environment**: Keil MDK-ARM
- **BLE Stack**: RivieraWaves BLE Stack

## Project Structure

```
Code/
└── 电子烟机-V1.01/
    └── SDK/
        ├── projects/ble_app_gatt/    # Main application project
        │   └── app/
        │       ├── app.c              # Application entry point
        │       ├── app_fff0.c/h       # Custom BLE service (FFF0)
        │       ├── app_task.c/h       # Application task management
        │       └── ...
        ├── libs/                      # Application libraries
        │   ├── drv/                   # Hardware drivers
        │   │   ├── app_motor.c/h      # Motor control driver
        │   │   ├── drv_battery.c/h    # Battery monitoring
        │   │   ├── drv_charger.c/h    # Charging control
        │   │   ├── drv_load.c/h       # Load control (GPIO)
        │   │   └── drv_rgb.c/h        # RGB LED driver
        │   ├── AT_Module/             # AT command interface
        │   ├── Password/              # Password management
        │   └── sys_manager.c/h        # System state management
        ├── ble_stack/                 # BLE protocol stack
        └── sdk/                       # SDK core components
```

## Building

1. Open `Code/电子烟机-V1.01/SDK/projects/ble_app_gatt/bk3432.uvproj` in Keil MDK-ARM
2. Select target configuration
3. Build the project (F7)
4. Flash to BK3432 via J-Link or UART

## License

This project is licensed under the MIT License.
