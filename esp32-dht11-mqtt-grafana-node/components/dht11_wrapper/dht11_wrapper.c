#include "dht11_wrapper.h"

#include "dht.h"

esp_err_t Dht11WrapperRead(gpio_num_t pin, float *temperatureC, float *humidityPct)
{
    if (temperatureC == NULL || humidityPct == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    return dht_read_float_data(DHT_TYPE_DHT11, pin, humidityPct, temperatureC);
}
