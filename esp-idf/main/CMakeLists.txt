# mDash sources are taken from the repo root, so the same mDash.c and
# mongoose.c files are shared between posix, Arduino and IDF builds
idf_component_register(SRCS "main.c"
                            "../../src/mDash.c"
                            "../../src/mongoose.c"
                       INCLUDE_DIRS "." "../../src"
                       REQUIRES esp_wifi esp_event esp_netif esp_timer
                                 nvs_flash spiffs app_update)
