idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash cjson wifi_manager mqtt_manager esp_adc thermistor_wrapper light_sensor_wrapper soil_sensor_wrapper)
