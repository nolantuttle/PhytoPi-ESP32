#include "soil_sensor_wrapper.h"

#define SOIL_CHANNEL    ADC_CHANNEL_0   // GPIO1 on ESP32-S3

// calibrate by reading raw ADC in open air (dry) and submerged (wet)
#define SOIL_DRY_RAW    350
#define SOIL_WET_RAW    280

static adc_oneshot_unit_handle_t s_adc1 = NULL;

esp_err_t SoilSensorWrapperInit(adc_oneshot_unit_handle_t adc1_handle)
{
    if (adc1_handle == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    adc_oneshot_chan_cfg_t cfg = {
        .atten    = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    esp_err_t ret = adc_oneshot_config_channel(adc1_handle, SOIL_CHANNEL, &cfg);
    if (ret == ESP_OK)
    {
        s_adc1 = adc1_handle;
    }
    return ret;
}

esp_err_t SoilSensorWrapperRead(float *moisturePct)
{
    if (moisturePct == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_adc1 == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    int raw = 0;
    esp_err_t ret = adc_oneshot_read(s_adc1, SOIL_CHANNEL, &raw);
    if (ret != ESP_OK)
    {
        return ret;
    }

    // capacitive sensor output is inverted — higher voltage means drier soil
    float pct = (float)(SOIL_DRY_RAW - raw) / (float)(SOIL_DRY_RAW - SOIL_WET_RAW) * 100.0f;
    if (pct < 0.0f)   pct = 0.0f;
    if (pct > 100.0f) pct = 100.0f;

    *moisturePct = pct;
    return ESP_OK;
}
