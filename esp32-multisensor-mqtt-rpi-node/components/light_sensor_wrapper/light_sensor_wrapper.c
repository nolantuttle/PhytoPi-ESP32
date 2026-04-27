#include "light_sensor_wrapper.h"

#define LIGHT_CHANNEL       ADC_CHANNEL_1   // GPIO2 on ESP32-S3
#define ADC_MAX_RAW         4095.0f

static adc_oneshot_unit_handle_t s_adc1 = NULL;

esp_err_t LightSensorWrapperInit(adc_oneshot_unit_handle_t adc1_handle)
{
    if (adc1_handle == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    adc_oneshot_chan_cfg_t cfg = {
        .atten    = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    esp_err_t ret = adc_oneshot_config_channel(adc1_handle, LIGHT_CHANNEL, &cfg);
    if (ret == ESP_OK)
    {
        s_adc1 = adc1_handle;
    }
    return ret;
}

esp_err_t LightSensorWrapperRead(float *lightPct)
{
    if (lightPct == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_adc1 == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    int raw = 0;
    esp_err_t ret = adc_oneshot_read(s_adc1, LIGHT_CHANNEL, &raw);
    if (ret != ESP_OK)
    {
        return ret;
    }

    float pct = (float)raw / ADC_MAX_RAW * 100.0f;
    if (pct < 0.0f)   pct = 0.0f;
    if (pct > 100.0f) pct = 100.0f;

    *lightPct = pct;
    return ESP_OK;
}
