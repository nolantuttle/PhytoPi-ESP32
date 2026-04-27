// Daniel Grijalva
// SWE-410 Milestone 3
// mqtt_manager — broker connection, event handler, publish function

#include "mqtt_manager.h"

#include "esp_log.h"
#include "mqtt_client.h"

#define MQTT_TOPIC_SUB  "gcu/swe410/dht11/commands"

static const char *TAG = "MQTT_MANAGER";

static esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT connected to broker");
        esp_mqtt_client_subscribe(client, MQTT_TOPIC_SUB, 1);
        ESP_LOGI(TAG, "Subscribed to %s", MQTT_TOPIC_SUB);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "MQTT disconnected");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT subscribe confirmed msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT message received");
        ESP_LOGI(TAG, "  topic: %.*s", event->topic_len, event->topic);
        ESP_LOGI(TAG, "  data:  %.*s", event->data_len, event->data);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT error");
        if (event->error_handle) {
            ESP_LOGE(TAG, "  tls error:    0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGE(TAG, "  tls stack:    %d",   event->error_handle->esp_tls_stack_err);
            ESP_LOGE(TAG, "  socket errno: %d",   event->error_handle->esp_transport_sock_errno);
        }
        break;

    default:
        break;
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri               = CONFIG_MQTT_MANAGER_BROKER_URI,
        .credentials.username             = CONFIG_MQTT_MANAGER_USERNAME,
        .credentials.authentication.password = CONFIG_MQTT_MANAGER_PASSWORD,
    };

    ESP_LOGI(TAG, "Connecting to broker: %s", CONFIG_MQTT_MANAGER_BROKER_URI);

    client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(
        client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));
}

void mqtt_publish(const char *topic, const char *data)
{
    if (client != NULL) {
        esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
    }
}
