#ifndef I2S_H
#define I2S_H

#include <driver/i2s_std.h>

#include <esp_err.h>

#define I2S_SCK GPIO_NUM_39
#define I2S_WS  GPIO_NUM_40 
#define I2S_SD  GPIO_NUM_9

#define I2S_SAMPLE_RATE  (8000U)
#define I2S_SAMPLE_BYTES (4U)
#define I2S_SAMPLE_COUNT (16384U)

extern i2s_chan_handle_t s_rx_handle;

void mic_init();
void mic_deinit();
size_t mic_read(int16_t* samples, size_t max_samples);

#endif
