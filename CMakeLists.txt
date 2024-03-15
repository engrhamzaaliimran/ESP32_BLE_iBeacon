set(srcs "src/ibeacon.c"
        "src/esp_ibeacon_api.c"
        "src/uuid.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS include
                    REQUIRES bt)
