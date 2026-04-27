// Daniel Grijalva
// SWE-410 Milestone 3
// DHT11 wrapper implementation

#include "dht11_wrapper.h"

#include "driver/gpio.h"
#include "dht.h"
#include "esp_log.h"

static const char *TAG = "DHT11_WRAPPER";

// GPIO pin the DHT11 data line is connected to (set by dht11_init)
static gpio_num_t s_pin = GPIO_NUM_NC;

static float c_to_f(float tempc)
{
    return (tempc * 9.0f / 5.0f) + 32.0f;
}

void dht11_init(gpio_num_t pin)
{
    s_pin = pin;
    // Bus idles high; esp-idf-lib DHT uses open-drain drive. Enable the
    // internal pull-up (weak). Many breakout boards still need a 4.7k–10k
    // resistor from DATA to 3V3 for reliable edges when Wi-Fi is active.
    gpio_reset_pin(pin);
    gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);

    ESP_LOGI(TAG, "Initialized DHT11 on GPIO %d", (int)s_pin);
}

esp_err_t dht11_read(dht11_reading_t *out)
{
    if (s_pin == GPIO_NUM_NC) {
        ESP_LOGE(TAG, "DHT11 not initialized — call dht11_init() first");
        return ESP_FAIL;
    }

    if (out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    float humidity = 0.0f;
    float temperature_c = 0.0f;

    esp_err_t err = dht_read_float_data(
        DHT_TYPE_DHT11,
        s_pin,
        &humidity,
        &temperature_c
    );

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "dht_read_float_data failed: %d", err);
        return err;
    }

    out->humidity = humidity;
    out->temperature_c = temperature_c;
    out->temperature_f = c_to_f(temperature_c);

    return ESP_OK;
}
