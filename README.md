# ESP32_BoardCfg
------

This library provides a simple way to manage board-specific, persistent configurations for ESP32-based projects. 

The provided example can be built using platformio, with complete setup provided.

## Library Dependencies
- [Preferences](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html#nvs-flash) - for persistent storage of configuration data.
- [UUIDv7] - for generating unique identifiers for devices or configurations.
