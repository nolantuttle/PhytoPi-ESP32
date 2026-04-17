#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_err.h"

#include "cJSON.h"

#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "dht11_wrapper.h"

#define WIFI_SSID "nolan_wifi"
#define WIFI_PASSWORD "rootroot"

#define MQTT_BROKER_URI "mqtt://192.168.50.1:1883"
#define MQTT_USERNAME "nolan"
#define MQTT_PASSWORD "rootroot"
#define MQTT_TOPIC "gcu/lab/env/telemetry"

#define DHT_GPIO GPIO_NUM_5

static const char *TAG = "SMART_LAB_NODE";

static void PublishTelemetryTask(void *parameter)
{
    ESP_LOGI(TAG, "Telemetry task started. Waiting for WiFi...");
    WifiManagerWaitConnected(0);

    ESP_LOGI(TAG, "WiFi ready. Initializing MQTT...");
    vTaskDelay(pdMS_TO_TICKS(2000));
    MqttManagerStart(MQTT_BROKER_URI, MQTT_USERNAME, MQTT_PASSWORD);
    ESP_LOGI(TAG, "Entering telemetry loop.");

    while (1)
    {
        float humidity = 0.0f;
        float temperature = 0.0f;

        esp_err_t result = Dht11WrapperRead(DHT_GPIO, &temperature, &humidity);

        if (result == ESP_OK)
        {
            cJSON *rootObject = cJSON_CreateObject();
            if (rootObject != NULL)
            {
                cJSON_AddStringToObject(rootObject, "device", "esp32-01");
                cJSON_AddNumberToObject(rootObject, "temperature", temperature);
                cJSON_AddNumberToObject(rootObject, "humidity", humidity);

                char *jsonMessage = cJSON_PrintUnformatted(rootObject);
                if (jsonMessage != NULL)
                {
                    int messageId = MqttManagerPublish(MQTT_TOPIC, jsonMessage, 1, 0);
                    ESP_LOGI(TAG, "Published msg_id=%d: %s", messageId, jsonMessage);
                    cJSON_free(jsonMessage);
                }
                cJSON_Delete(rootObject);
            }
        }
        else
        {
            ESP_LOGW(TAG, "Failed to read DHT11 sensor.");
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "app_main start. Reset reason: %d", esp_reset_reason());

    esp_err_t flashResult = nvs_flash_init();
    if (flashResult == ESP_ERR_NVS_NO_FREE_PAGES ||
        flashResult == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        flashResult = nvs_flash_init();
    }
    ESP_ERROR_CHECK(flashResult);

    WifiManagerInit(WIFI_SSID, WIFI_PASSWORD);

    xTaskCreate(PublishTelemetryTask, "PublishTelemetryTask", 8192, NULL, 5, NULL);
}
