#pragma once

#include "esp_err.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t Dht11WrapperRead(gpio_num_t pin, float *temperatureC, float *humidityPct);

#ifdef __cplusplus
}
#endif
