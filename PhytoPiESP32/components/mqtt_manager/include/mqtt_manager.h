#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

// Connect to the MQTT broker and register event handlers.
// Call this after Wi-Fi is connected.
void mqtt_app_start(void);

// Publish a null-terminated string payload to the given topic.
// QoS 1, no retain. Safe to call only after mqtt_app_start().
void mqtt_publish(const char *topic, const char *data);

#endif // MQTT_MANAGER_H
