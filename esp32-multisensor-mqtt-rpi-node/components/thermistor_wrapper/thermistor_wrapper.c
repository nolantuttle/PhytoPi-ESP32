#include "thermistor_wrapper.h"

#include "esp_log.h"
#include <math.h>

#define THERMISTOR_CHANNEL  ADC_CHANNEL_4   // GPIO5 on ESP32-S3

// 10k NTC, B=3950, 10k series resistor to 3.3V — adjust if your circuit differs
#define THERMISTOR_BETA     3950.0f
#define THERMISTOR_R25      10000.0f
#define THERMISTOR_RSERIES  10000.0f
#define VSUPPLY_MV          3300.0f
#define ADC_FULL_SCALE_MV   3100.0f         // DB12 attenuation ~3100mV full scale on ESP32-S3
#define ADC_MAX_RAW         4095.0f
#define KELVIN_OFFSET       273.15f
#define T25_KELVIN          (25.0f + KELVIN_OFFSET)

static const char *TAG = "thermistor_wrapper";
static adc_oneshot_unit_handle_t s_adc1 = NULL;

esp_err_t ThermistorWrapperInit(adc_oneshot_unit_handle_t adc1_handle)
{
    if (adc1_handle == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    adc_oneshot_chan_cfg_t cfg = {
        .atten    = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    esp_err_t ret = adc_oneshot_config_channel(adc1_handle, THERMISTOR_CHANNEL, &cfg);
    if (ret == ESP_OK)
    {
        s_adc1 = adc1_handle;
    }
    return ret;
}

esp_err_t ThermistorWrapperRead(float *temperatureC)
{
    if (temperatureC == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_adc1 == NULL)
    {
        return ESP_ERR_INVALID_STATE;
    }

    int raw = 0;
    esp_err_t ret = adc_oneshot_read(s_adc1, THERMISTOR_CHANNEL, &raw);
    if (ret != ESP_OK)
    {
        return ret;
    }

    float v_mv = (float)raw / ADC_MAX_RAW * ADC_FULL_SCALE_MV;

    if (v_mv <= 0.0f || v_mv >= VSUPPLY_MV)
    {
        ESP_LOGW(TAG, "raw %d out of usable range", raw);
        return ESP_FAIL;
    }

    // voltage divider: VCC -> R_series -> ADC node -> R_therm -> GND
    float r_therm = THERMISTOR_RSERIES * v_mv / (VSUPPLY_MV - v_mv);
    float temp_k  = 1.0f / (1.0f / T25_KELVIN + (1.0f / THERMISTOR_BETA) * logf(r_therm / THERMISTOR_R25));
    *temperatureC = temp_k - KELVIN_OFFSET;

    return ESP_OK;
}
