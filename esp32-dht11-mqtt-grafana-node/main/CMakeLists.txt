idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES nvs_flash json wifi_manager mqtt_manager dht11_wrapper)
