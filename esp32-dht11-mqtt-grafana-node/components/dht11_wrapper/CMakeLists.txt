idf_component_register(
    SRCS "dht11_wrapper.c"
    INCLUDE_DIRS "."
    REQUIRES driver
    PRIV_REQUIRES esp32-dht)
