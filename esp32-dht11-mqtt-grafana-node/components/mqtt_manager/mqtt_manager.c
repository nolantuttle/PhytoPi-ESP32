#include "mqtt_manager.h"

#include "esp_log.h"
#include "mqtt_client.h"

static const char *TAG = "mqtt_manager";

static esp_mqtt_client_handle_t MqttClient = NULL;
static bool MqttConnected = false;

static void MqttEventHandler(
    void *handlerArgs,
    esp_event_base_t base,
    int32_t eventId,
    void *eventData)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)eventData;

    switch ((esp_mqtt_event_id_t)eventId)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Connected to MQTT broker.");
        MqttConnected = true;
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Disconnected from MQTT broker.");
        MqttConnected = false;
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT message published successfully.");
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT event error. type=%d", event->error_handle->error_type);
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            ESP_LOGE(TAG, "TCP error: esp_tls=%d sock=%d",
                     event->error_handle->esp_tls_last_esp_err,
                     event->error_handle->esp_transport_sock_errno);
        }
        break;
    default:
        break;
    }
}

esp_err_t MqttManagerStart(
    const char *brokerUri,
    const char *username,
    const char *password)
{
    esp_mqtt_client_config_t mqttConfiguration = {
        .broker.address.uri = brokerUri,
        .credentials.username = username,
        .credentials.authentication.password = password,
        .task.stack_size = 8192};

    MqttClient = esp_mqtt_client_init(&mqttConfiguration);
    if (MqttClient == NULL)
    {
        ESP_LOGE(TAG, "Failed to init MQTT client.");
        return ESP_FAIL;
    }

    esp_mqtt_client_register_event(MqttClient, ESP_EVENT_ANY_ID, MqttEventHandler, NULL);
    return esp_mqtt_client_start(MqttClient);
}

int MqttManagerPublish(
    const char *topic,
    const char *payload,
    int qos,
    int retain)
{
    if (MqttClient == NULL)
    {
        return -1;
    }
    return esp_mqtt_client_publish(MqttClient, topic, payload, 0, qos, retain);
}

bool MqttManagerIsReady(void)
{
    return MqttConnected;
}
