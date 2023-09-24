idf_component_register(SRCS "eltersdorftemp_main.c" "i2c.c" "lps35hw.c" "network.c" "rg15.c" "sen50.c" "sht4x.c" "webserver.c"
                       INCLUDE_DIRS ""
	               REQUIRES soc esp_http_server app_update esp_https_ota nvs_flash esp_wifi esp_netif lwip mbedtls driver)

