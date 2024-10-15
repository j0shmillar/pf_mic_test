#include <PowerFeather.h>

#include <freertos/FreeRTOS.h>

#include <driver/rtc_io.h>
#include <driver/gpio.h>

#include <esp_task_wdt.h>
#include <esp_sleep.h>
#include <esp_timer.h>

#include "mic.h"

#define SAMPLE_RATE 16000
#define RECORD_DURATION_SECONDS 3
#define SLEEP_DURATION_SECONDS 15

RTC_DATA_ATTR uint64_t sleep_start_time = 0;

void configure_sleep(uint64_t sleep_duration_us) 
{
    PowerFeather::Board.setEN(false); // mic - power down
    PowerFeather::Board.enableVSQT(false); // other v sources
    esp_sleep_enable_timer_wakeup(sleep_duration_us); // enable timer wake-up
}

void capture_audio() {
    PowerFeather::Board.setEN(true); // mic - power on
    int16_t* buffer = (int16_t*)malloc(SAMPLE_RATE * sizeof(int16_t));
    while (buffer == NULL) // TODO fix - chance of getting stuck here
    {
        printf("buffer allocation failed");
        continue;
    }
    
    mic_init();
    size_t total_samples = 0;
    int64_t start_time = esp_timer_get_time();

    printf("started recording...\n");
    while (esp_timer_get_time() - start_time < RECORD_DURATION_SECONDS * 1000000) 
    {
        int samples_read = mic_read(buffer + total_samples, SAMPLE_RATE - total_samples);
        total_samples += samples_read;
        
        printf("mic_read: %d samples, total: %d samples\n", samples_read, total_samples);

        esp_task_wdt_reset(); 

        vTaskDelay(pdMS_TO_TICKS(10));
    }
    printf("finished recording\n");
    
    mic_deinit();
    free(buffer);
}

void record_task(void *pvParameters) 
{
    esp_task_wdt_add(NULL); 
    while (1) 
    {
        if (!(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0))
        {
            capture_audio();
        }
        esp_task_wdt_reset(); 
        vTaskDelay(pdMS_TO_TICKS(100));

        sleep_start_time = esp_timer_get_time();
        configure_sleep(SLEEP_DURATION_SECONDS * 1000000);
        esp_deep_sleep_start();
    }
}

extern "C" void app_main()
{
    if (PowerFeather::Board.init(0) == PowerFeather::Result::Ok)
    {
        PowerFeather::Board.enableBatteryCharging(false);
        PowerFeather::Board.enableBatteryFuelGauge(false);
        PowerFeather::Board.enableBatteryTempSense(false);
        printf("board init success\n");
    }

    static bool wdt_initialized = false;
    if (!wdt_initialized) 
    {
        esp_task_wdt_config_t wdt_config = 
        {
            .timeout_ms = 30000,
            .idle_core_mask = (1 << 0) | (1 << 1),
            .trigger_panic = true
        };
        esp_task_wdt_init(&wdt_config);
        wdt_initialized = true; 
    }

    BaseType_t result = xTaskCreatePinnedToCore(record_task, "Record Task", 4096, NULL, 1, NULL, 0);
    if (result == pdPASS) 
    {
        printf("task created\n");
    } 
    else 
    {
        printf("task creation failed\n");
    }
}
