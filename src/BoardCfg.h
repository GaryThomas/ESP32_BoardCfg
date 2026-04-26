// Copyright 2026 MLB Associates, LLC
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

#ifndef _BOARDCFG_H
#define _BOARDCFG_H

#include "Arduino.h"
#include <Preferences.h>

class BoardCfg {
  private:
    bool _started;
    bool _readOnly;
    bool _dirty;
    Preferences _prefs;

  public:
    BoardCfg();
    ~BoardCfg();

    bool begin();
    void end();
    void reset();
    void update();
    void dump(const char *title = nullptr);

    void setDeviceName(const char *name) {
        if (strlen(name) < sizeof(deviceName)) {
            strncpy(deviceName, name, sizeof(deviceName));
            deviceName[sizeof(deviceName) - 1] = '\0'; // Ensure null termination
            _dirty = true;
        }
    }

    // Board configuration fields - stores in NVS
    char guid[37]; // 36 chars + null terminator
    char deviceName[32];
    bool valid; // Add a validity flag to detect valid config
};

#endif //_BOARDCFG_H
