// Daniel Grijalva
// SWE-410 Milestone 3
// ESP32 DHT11 sensor integration and MQTT telemetry to Raspberry Pi 5

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "nvs_flash.h"

#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "dht11_wrapper.h"

static const char *TAG = "MAIN";

#define DHT11_DATA_PIN      GPIO_NUM_4
#define MQTT_TOPIC_TELEMETRY "gcu/swe410/dht11/telemetry"

void app_main(void)
{
    // Initialize non-volatile storage — required by Wi-Fi driver
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Start Wi-Fi and block until an IP is assigned
    wifi_init_sta();
    wifi_wait_connected();

    // Connect to the MQTT broker
    mqtt_app_start();

    // Initialize the DHT11 sensor on the configured GPIO pin
    dht11_init(DHT11_DATA_PIN);
    ESP_LOGI(TAG, "Starting DHT11 telemetry loop (GPIO %d)", (int)DHT11_DATA_PIN);

    dht11_reading_t reading;

    while (1) {
        esp_err_t err = dht11_read(&reading);

        if (err == ESP_OK) {
            // Serial log matches the lab project format
            ESP_LOGI(TAG, "Humidity: %.1f%%, Temp: %.1fF (%.1fC)",
                     reading.humidity, reading.temperature_f, reading.temperature_c);

            // MQTT payload: JSON with humidity and temperature_c only
            // (temperature_f can be derived on the Pi side)
            char payload[64];
            snprintf(payload, sizeof(payload),
                     "{\"humidity\":%.1f,\"temperature_c\":%.1f}",
                     reading.humidity, reading.temperature_c);

            mqtt_publish(MQTT_TOPIC_TELEMETRY, payload);
        } else {
            ESP_LOGW(TAG, "DHT11 read failed (err=%d), retrying in 2s", (int)err);
        }

        // DHT11 requires at least 1-2 s between reads
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
