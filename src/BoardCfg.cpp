// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "BoardCfg.h"
#include "UUID7.h"
#include <Preferences.h>

#include <nvs.h>
#include <nvs_flash.h>
#include "esp_chip_info.h"

BoardCfg::BoardCfg() : _started(false), _readOnly(false) {}

BoardCfg::~BoardCfg() { end(); }

// determined based on the constants defined in esp_chip_info.h
const char *get_chip_model(esp_chip_model_t model) {
    switch (model) {
    case CHIP_ESP32:
        return "ESP32";
    case CHIP_ESP32S2:
        return "ESP32-S2";
    case CHIP_ESP32S3:
        return "ESP32-S3";
    case CHIP_ESP32C3:
        return "ESP32-C3";
    case CHIP_ESP32C2:
        return "ESP32-C2";
    case CHIP_ESP32C6:
        return "ESP32-C6";
    case CHIP_ESP32H2:
        return "ESP32-H2";
    case CHIP_ESP32P4:
        return "ESP32-P4";
    case CHIP_POSIX_LINUX:
        return "POSIX/Linux Simulator";
    default:
        return "Unknown Model";
    }
}

bool BoardCfg::begin() {
    UUID7 uuid;
    if (_started) {
        return false;
    }
    valid = false;
    _readOnly = false;
    _dirty = false;
    // Look for existing config in NVS
    _prefs.begin("board_cfg", false);
    if (_prefs.isKey("valid") && _prefs.getBool("valid", true)) {
        String guidStr = _prefs.getString("guid", "");
        if (guidStr.length() == 36) {
            guidStr.toCharArray(guid, sizeof(guid));
        }
        String deviceNameStr = _prefs.getString("deviceName", "");
        deviceNameStr.toCharArray(deviceName, sizeof(deviceName));
        _started = true;
        _dirty = false;
        valid = true;
        return true;
    } else {
        Serial.println("No valid GUID found in NVS, generating new configuration");
    }
    // Generate a new configuration and save it to NVS
    _started = true;
    _dirty = true;
    valid = true;
    esp_chip_info_t chipInfo;
    esp_chip_info(&chipInfo);
    sprintf(deviceName, "%s.%04d", get_chip_model(chipInfo.model), random(1000, 9999));
    if (uuid.generate()) {
        uuid.toString(guid, sizeof(guid));
    } else {
        Serial.println("Failed to generate UUID for BoardCfg");
        guid[0] = '\0';
        return false;
    }
    update();
    return true;
}

void BoardCfg::end() {
    if (!_started) {
        return;
    }
    _started = false;
    _prefs.end();
}

void BoardCfg::dump(const char *title) {
    if (!_started) {
        return;
    }
    if (title) {
        Serial.println(title);
    } else {
        Serial.println("BoardCfg Dump:");
    }
    Serial.println("GUID: " + String(guid));
    Serial.println("Device Name: " + String(deviceName));
    Serial.println("Valid: " + String(valid));
}

void BoardCfg::reset() {
    _started = false;
    _prefs.end();
    nvs_flash_erase(); // Erase the entire NVS partition to reset all data
    Serial.println("NVS erased - BoardCfg reset to defaults");
}

void BoardCfg::update() {
    if (!_started || !_dirty) {
        return;
    }
    if (_readOnly) {
        Serial.println("BoardCfg is read-only, cannot update");
        return;
    }
    _prefs.putBool("valid", true); // Add a version key to detect valid config
    _prefs.putString("guid", guid);
    _prefs.putString("deviceName", deviceName);
    Serial.println("BoardCfg updated in NVS");
    _dirty = false;
}
