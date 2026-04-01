# 蓝牙电子烟机

[!\[License: MIT\](https://img.shields.io/badge/License-MIT-yellow.svg null)](https://opensource.org/licenses/MIT)

English | [中文](README_CN.md)

***

## 概述

这是一个基于 BLE（低功耗蓝牙）的电子烟机控制系统固件，专为 Beken BK3432 BLE SoC 平台设计。提供完整的硬件控制能力，包括电机控制、电池管理、充电控制和 RGB LED 效果。

## 功能特性

- **BLE 4.2** 基于 Beken BK3432 BLE SoC
- **电机控制**: 支持多达 10 个通道（旋转机 1-5，格子机 1-10）
- **电池管理**: 实时电压监测和低电量保护
- **充电控制**: 硬件充电使能/禁用控制
- **RGB LED 效果**: 支持 WS2812 RGB 灯带，可配置颜色
- **AT 命令接口**: 基于 UART 的 AT 命令接口，用于设备配置
- **OTA 支持**: 空中固件更新功能
- **密码保护**: 可配置的访问控制和密码认证

## 硬件平台

- **MCU**: Beken BK3432 (ARM Cortex-M4)
- **开发环境**: Keil MDK-ARM
- **BLE 协议栈**: RivieraWaves BLE Stack

## 项目结构

```
Code/
└── 电子烟机-V1.01/
    └── SDK/
        ├── projects/ble_app_gatt/    # 主应用程序项目
        │   └── app/
        │       ├── app.c              # 应用程序入口
        │       ├── app_fff0.c/h       # 自定义 BLE 服务 (FFF0)
        │       ├── app_task.c/h       # 应用程序任务管理
        │       └── ...
        ├── libs/                      # 应用程序库
        │   ├── drv/                   # 硬件驱动
        │   │   ├── app_motor.c/h      # 电机控制驱动
        │   │   ├── drv_battery.c/h    # 电池监测
        │   │   ├── drv_charger.c/h    # 充电控制
        │   │   ├── drv_load.c/h       # 负载控制 (GPIO)
        │   │   └── drv_rgb.c/h        # RGB LED 驱动
        │   ├── AT_Module/             # AT 命令接口
        │   ├── Password/              # 密码管理
        │   └── sys_manager.c/h        # 系统状态管理
        ├── ble_stack/                 # BLE 协议栈
        └── sdk/                       # SDK 核心组件
```

## 编译构建

1. 在 Keil MDK-ARM 中打开 `Code/电子烟机-V1.01/SDK/projects/ble_app_gatt/bk3432.uvproj`
2. 选择目标配置
3. 构建项目 (F7)
4. 通过 J-Link 或 UART 烧录到 BK3432

## 许可证

本项目采用 MIT 许可证。
