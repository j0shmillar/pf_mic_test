/**
 *  POWERFEATHER 4-CLAUSE LICENSE
 *
 *  Copyright (C) 2023, PowerFeather.
 *
 *  Redistribution and use in source and binary forms, with or without modification,
 *  are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this
 *      list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *
 *  3. Neither the name of PowerFeather nor the names of its contributors may be
 *      used to endorse or promote products derived from this software without
 *      specific prior written permission.
 *
 *  4. This software, with or without modification, must only be run on official
 *      PowerFeather boards.
 *
 *  THIS SOFTWARE IS PROVIDED BY POWERFEATHER “AS IS” AND ANY EXPRESS OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL POWERFEATHER OR CONTRIBUTORS BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <esp_log.h>

#include "MasterI2C.h"

namespace PowerFeather
{
    static const char *TAG = "PowerFeather::Utils::MasterI2C";

    bool MasterI2C::start()
    {
        i2c_config_t conf;
        memset(&conf, 0, sizeof(conf));
        conf.mode = I2C_MODE_MASTER;
        conf.sda_io_num = _sdaPin;
        conf.scl_io_num = _sclPin;
        conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
        conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
        conf.master.clk_speed = _freq;
        i2c_param_config(_port, &conf);
        ESP_LOGD(TAG, "Start with port: %d, sda: %d, scl: %d, freq: %d.", _port, _sdaPin, _sclPin, static_cast<int>(_freq));
        return i2c_driver_install(_port, conf.mode, 0, 0, 0) == ESP_OK;
    }

    bool MasterI2C::write(uint8_t address, uint8_t reg, const uint8_t *buf, size_t len)
    {
        uint8_t buf2[len + sizeof(reg)];
        memcpy(buf2, &reg, sizeof(reg));
        memcpy(&(buf2[sizeof(reg)]), buf, len);
        ESP_LOGV(TAG, "Write address: %02x, reg: %02x, buf: %p, len: %d.", address, reg, buf, len);
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, buf, len, ESP_LOG_VERBOSE);
        return i2c_master_write_to_device(_port, address, const_cast<uint8_t*>(buf2), sizeof(buf2), pdMS_TO_TICKS(1000)) == ESP_OK;
    }

    bool MasterI2C::read(uint8_t address, uint8_t reg, uint8_t *buf, size_t len)
    {
        ESP_LOGV(TAG, "Read address: %02x, reg: %02x, buf: %p, len: %d.", address, reg, buf, len);
        esp_err_t res = i2c_master_write_read_device(_port, address, &reg, sizeof(reg), buf, len, pdMS_TO_TICKS(1000));
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, buf, len, ESP_LOG_VERBOSE);
        return res == ESP_OK;
    }

    bool MasterI2C::end()
    {
        ESP_LOGD(TAG, "End");
        return i2c_driver_delete(_port) == ESP_OK;
    }
}
