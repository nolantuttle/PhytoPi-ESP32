#include "wifi_manager.h"

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"

#define WIFI_CONNECTED_BIT BIT0

static const char *TAG = "wifi_manager";

static EventGroupHandle_t WifiEventGroup;

static void WifiEventHandler(
    void *arg,
    esp_event_base_t eventBase,
    int32_t eventId,
    void *eventData)
{
    if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "Wi-Fi started. Connecting to hotspot...");
        esp_wifi_connect();
    }
    else if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGW(TAG, "Wi-Fi disconnected. Reconnecting...");
        xEventGroupClearBits(WifiEventGroup, WIFI_CONNECTED_BIT);
        esp_wifi_connect();
    }
    else if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)eventData;
        ESP_LOGI(TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(WifiEventGroup, WIFI_CONNECTED_BIT);
    }
}

esp_err_t WifiManagerInit(const char *ssid, const char *password)
{
    WifiEventGroup = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifiInitConfiguration = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifiInitConfiguration));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        WIFI_EVENT, ESP_EVENT_ANY_ID, &WifiEventHandler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(
        IP_EVENT, IP_EVENT_STA_GOT_IP, &WifiEventHandler, NULL, NULL));

    wifi_config_t wifiConfiguration = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK}};

    strncpy((char *)wifiConfiguration.sta.ssid, ssid, sizeof(wifiConfiguration.sta.ssid));
    strncpy((char *)wifiConfiguration.sta.password, password, sizeof(wifiConfiguration.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifiConfiguration));
    ESP_ERROR_CHECK(esp_wifi_start());

    return ESP_OK;
}

bool WifiManagerWaitConnected(uint32_t timeoutMs)
{
    TickType_t ticks = (timeoutMs == 0) ? portMAX_DELAY : pdMS_TO_TICKS(timeoutMs);
    EventBits_t bits = xEventGroupWaitBits(
        WifiEventGroup, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, ticks);
    return (bits & WIFI_CONNECTED_BIT) != 0;
}
