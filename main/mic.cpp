#include <freertos/FreeRTOS.h>
#include "mic.h"

#include <esp_log.h>

static const char* TAG = "mic";
i2s_chan_handle_t s_rx_handle;

void mic_init() 
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, NULL, &s_rx_handle));

    i2s_std_config_t std_cfg = 
    {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_SCK,
            .ws = I2S_WS,
            .dout = I2S_GPIO_UNUSED,
            .din = I2S_SD,
            .invert_flags = 
            {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(s_rx_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(s_rx_handle));
}

void mic_deinit() 
{
    i2s_channel_disable(s_rx_handle);
    i2s_del_channel(s_rx_handle);
}

size_t mic_read(int16_t* samples, size_t max_samples) 
{
    static const size_t s_buffer_size = 1024U;
    static int32_t s_buffer[1024U];

    size_t count = max_samples;
    size_t sample_index = 0;

    while (count > 0) 
    {
        size_t bytes_read = 0;
        if (i2s_channel_read(s_rx_handle, (char*)s_buffer, s_buffer_size * I2S_SAMPLE_BYTES, &bytes_read, portMAX_DELAY) != ESP_OK) 
        {
            ESP_LOGE(TAG, "unable to read from audio channel");
            return sample_index;
        }

        size_t samples_read = bytes_read / I2S_SAMPLE_BYTES;
        for (size_t i = 0; i < samples_read && count > 0; ++i) 
        {
            samples[sample_index++] = s_buffer[i] >> 8;
            // printf("Sample %zu: %d\n", sample_index, samples[sample_index]);
            count--;
        }
        // vTaskDelay(pdMS_TO_TICKS(10));
    }

    return sample_index;
}
