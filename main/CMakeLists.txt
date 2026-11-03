idf_component_register(
										SRCS "main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES
												freertos
												esp_common
												esp_driver_gpio
												esp_driver_i2s
												esp_driver_i2c
												esp_driver_sdmmc
												sdmmc
												fatfs
												lcd1602
												ultrasonic
												dht
)