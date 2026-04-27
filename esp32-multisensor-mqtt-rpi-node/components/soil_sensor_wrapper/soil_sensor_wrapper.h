#pragma once

#include "esp_err.h"
#include "esp_adc/adc_oneshot.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t SoilSensorWrapperInit(adc_oneshot_unit_handle_t adc1_handle);
esp_err_t SoilSensorWrapperRead(float *moisturePct);

#ifdef __cplusplus
}
#endif
