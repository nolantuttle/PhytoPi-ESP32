#pragma once

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t WifiManagerInit(const char *ssid, const char *password);

bool WifiManagerWaitConnected(uint32_t timeoutMs);

#ifdef __cplusplus
}
#endif
