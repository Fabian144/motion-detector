#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

// Speaker
#include "driver/i2s_std.h"

// SD card
#include "driver/sdmmc_host.h"
#include "sdmmc_cmd.h"
#include "esp_vfs_fat.h"

// LCD display
#include "lcd1602/lcd1602.h"
#include "driver/i2c_master.h"

// Ultrasonic range meter
#include "ultrasonic.h"
#include "esp_err.h"

// Hygrothermograph
#include "dht.h"

#if defined(CONFIG_TYPE_DHT11)
static const dht_sensor_type_t HYGRO_TYPE = DHT_TYPE_DHT11;
#elif defined(CONFIG_TYPE_AM2301)
static const dht_sensor_type_t HYGRO_TYPE = DHT_TYPE_AM2301;
#elif defined(CONFIG_TYPE_SI7021)
static const dht_sensor_type_t HYGRO_TYPE = DHT_TYPE_SI7021;
#endif

// static const gpio_num_t TRIGGER_GPIO = CONFIG_TRIGGER_GPIO;
// static const gpio_num_t ECHO_GPIO = CONFIG_ECHO_GPIO;
// static const int MAX_ULTRASONIC_DISTANCE = CONFIG_MAX_DISTANCE;

// static const gpio_num_t POTENTIOMETER_GPIO = CONFIG_POTENTIOMETER_GPIO;

// static const gpio_num_t BUZZER_GPIO = CONFIG_BUZZER_GPIO;

static const gpio_num_t HYGRO_GPIO = CONFIG_HYGRO_GPIO;

void print_dht_data(void *pvParameters)
{
    float temperature, humidity;

#ifdef CONFIG_INTERNAL_PULLUP
    gpio_set_pull_mode(HYGRO_GPIO, GPIO_PULLUP_ONLY);
#endif

    while (1)
    {
        if (dht_read_float_data(HYGRO_TYPE, HYGRO_GPIO, &humidity, &temperature) == ESP_OK)
            printf("Humidity: %.1f%% Temp: %.1fC\n", humidity, temperature);
        else
            printf("Could not read data from sensor\n");

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main()
{ xTaskCreate(print_dht_data, "print_dht_data", 2048, NULL, 5, NULL); }
