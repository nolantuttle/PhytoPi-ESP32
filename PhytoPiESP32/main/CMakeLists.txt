idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES wifi_manager mqtt_manager dht11_wrapper nvs_flash
)
