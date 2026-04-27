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
#include "esp_adc/adc_oneshot.h"

#include "cJSON.h"

#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "thermistor_wrapper.h"
#include "light_sensor_wrapper.h"
#include "soil_sensor_wrapper.h"

#define WIFI_SSID "POD"
#define WIFI_PASSWORD "crispytulip468"

#define MQTT_BROKER_URI "mqtt://192.168.1.178:1883"
#define MQTT_USERNAME "nolan"
#define MQTT_PASSWORD "rootroot"
#define MQTT_TOPIC "gcu/lab/env/telemetry"

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
        float temperatureC = 0.0f;
        float lightPct = 0.0f;
        float moisturePct = 0.0f;

        esp_err_t tempResult = ThermistorWrapperRead(&temperatureC);
        esp_err_t lightResult = LightSensorWrapperRead(&lightPct);
        esp_err_t moistureResult = SoilSensorWrapperRead(&moisturePct);

        if (tempResult == ESP_OK && lightResult == ESP_OK && moistureResult == ESP_OK)
        {
            cJSON *rootObject = cJSON_CreateObject();
            if (rootObject != NULL)
            {
                cJSON_AddStringToObject(rootObject, "device", "esp32-01");
                cJSON_AddNumberToObject(rootObject, "temperature_c", temperatureC);
                cJSON_AddNumberToObject(rootObject, "light_pct", lightPct);
                cJSON_AddNumberToObject(rootObject, "soil_moisture_pct", moisturePct);

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
            ESP_LOGW(TAG, "Sensor read failed — temp:%d light:%d soil:%d",
                     tempResult, lightResult, moistureResult);
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

    adc_oneshot_unit_init_cfg_t adc1_cfg = {.unit_id = ADC_UNIT_1};
    adc_oneshot_unit_handle_t adc1_handle = NULL;
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc1_cfg, &adc1_handle));

    ESP_ERROR_CHECK(ThermistorWrapperInit(adc1_handle));
    ESP_ERROR_CHECK(LightSensorWrapperInit(adc1_handle));
    ESP_ERROR_CHECK(SoilSensorWrapperInit(adc1_handle));

    xTaskCreate(PublishTelemetryTask, "PublishTelemetryTask", 8192, NULL, 5, NULL);
}
