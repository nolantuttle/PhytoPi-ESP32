#pragma once

#include "esp_err.h"
#include "esp_adc/adc_oneshot.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t LightSensorWrapperInit(adc_oneshot_unit_handle_t adc1_handle);
esp_err_t LightSensorWrapperRead(float *lightPct);

#ifdef __cplusplus
}
#endif
