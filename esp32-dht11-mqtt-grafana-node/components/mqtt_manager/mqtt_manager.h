#pragma once

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t MqttManagerStart(
    const char *brokerUri,
    const char *username,
    const char *password);

int MqttManagerPublish(
    const char *topic,
    const char *payload,
    int qos,
    int retain);

bool MqttManagerIsReady(void);

#ifdef __cplusplus
}
#endif
