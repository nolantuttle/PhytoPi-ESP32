#pragma once

#include "esp_err.h"
#include "esp_adc/adc_oneshot.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ThermistorWrapperInit(adc_oneshot_unit_handle_t adc1_handle);
esp_err_t ThermistorWrapperRead(float *temperatureC);

#ifdef __cplusplus
}
#endif
