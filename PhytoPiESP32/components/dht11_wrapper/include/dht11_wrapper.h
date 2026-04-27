// Daniel Grijalva
// SWE-410 Milestone 3
// DHT11 wrapper public API

#ifndef DHT11_WRAPPER_H
#define DHT11_WRAPPER_H

#include "driver/gpio.h"
#include "esp_err.h"

// Groups a complete DHT11 sensor reading into a single object
typedef struct
{
    float humidity;       // Relative humidity (%)
    float temperature_c;  // Temperature in Celsius
    float temperature_f;  // Temperature in Fahrenheit
} dht11_reading_t;

// Store which GPIO pin the DHT11 data line is connected to
void dht11_init(gpio_num_t pin);

// Read humidity and temperature into *out
// Returns ESP_OK on success, error code otherwise
esp_err_t dht11_read(dht11_reading_t *out);

#endif // DHT11_WRAPPER_H
