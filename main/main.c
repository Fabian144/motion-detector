#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
// #include "driver/i2s_std.h"		// Speaker
// #include "driver/sdmmc_host.h" // SD
// #include "sdmmc_cmd.h"					// SD
// #include "esp_vfs_fat.h"				// SD
#include "dht.h"             // Hygro
#include "lcd1602/lcd1602.h" // LCD
#include "ultrasonic.h"      // Ultrasonic
#include "esp_err.h"         // Ultrasonic

// #define POTENTIOMETER_GPIO 			CONFIG_POTENTIOMETER_GPIO
// #define BUZZER_GPIO 							CONFIG_BUZZER_GPIO
#define HYGRO_GPIO      CONFIG_HYGRO_GPIO
#define ESP_I2C_PORT    CONFIG_ESP_I2C_PORT
#define ESP_I2C_SDA     CONFIG_ESP_I2C_SDA
#define ESP_I2C_SCL     CONFIG_ESP_I2C_SCL
#define ESP_I2C_ADDRESS LCD1602_I2C_ADDRESS_DEFAULT
#define TRIGGER_GPIO    CONFIG_TRIGGER_GPIO
#define ECHO_GPIO       CONFIG_ECHO_GPIO
#define MAX_DISTANCE_CM CONFIG_MAX_DISTANCE

#if defined(CONFIG_TYPE_DHT11)
#define HYGRO_TYPE DHT_TYPE_DHT11
#elif defined(CONFIG_TYPE_AM2301)
#define HYGRO_TYPE DHT_TYPE_AM2301
#elif defined(CONFIG_TYPE_SI7021)
#define HYGRO_TYPE DHT_TYPE_SI7021
#endif

int16_t temperature, humidity;
lcd1602_context *ctx;
float current_max_distance = MAX_DISTANCE_CM;

void set_dht_data(void *pvParameters);
void display_dht_data(void *pvParameters);
void detect_motion(void *pvParameters);
void display_max_distance(void *pvParameters);

void app_main()
{
    i2c_lowlevel_config config = { 0 };
    config.port = ESP_I2C_PORT;
    config.pin_sda = ESP_I2C_SDA;
    config.pin_scl = ESP_I2C_SCL;

    ctx = lcd1602_init(ESP_I2C_ADDRESS, true, &config);
    if (ctx == NULL)
    {
        printf("Could not initialize LCD display\n");
    }

    xTaskCreate(detect_motion, "detect motion with ultrasonic sensor", 4096, NULL, 4, NULL);
    xTaskCreate(set_dht_data, "set dht data", 2048, NULL, 5, NULL);
    xTaskCreate(display_dht_data, "show dht data on the lcd display", 4096, NULL, 6, NULL);
    xTaskCreate(display_max_distance, "show max supersonic distance on the lcd display", 2048, NULL, 7, NULL);
}

void set_dht_data(void *pvParameters)
{
#ifdef CONFIG_INTERNAL_PULLUP
    gpio_set_pull_mode(HYGRO_GPIO, GPIO_PULLUP_ONLY);
#endif

    while (1)
    {
        if (dht_read_data(HYGRO_TYPE, HYGRO_GPIO, &humidity, &temperature) != ESP_OK)
        {
            printf("Could not read data from sensor\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void display_dht_data(void *pvParameters)
{
    char line[32];

    while (1)
    {
        snprintf(line, sizeof line, "Temp: %dC", temperature / 10);
        lcd1602_set_cursor(ctx, 0, 0);
        lcd1602_string(ctx, line);

        snprintf(line, sizeof line, "Hum:  %d%%", humidity / 10);
        lcd1602_set_cursor(ctx, 1, 0);
        lcd1602_string(ctx, line);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void display_max_distance(void *pvParameters)
{
    char line[32];

    while (1)
    {
        float current_max_distance_m = current_max_distance / 100;
				
        snprintf(line, sizeof line, "%.1fM", current_max_distance_m);
        lcd1602_set_cursor(ctx, 0, 12);
        lcd1602_string(ctx, line);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void detect_motion(void *pvParameters)
{
    ultrasonic_sensor_t sensor = { TRIGGER_GPIO, ECHO_GPIO };

    ultrasonic_init(&sensor);

    while (true)
    {
        float distance;
        esp_err_t res = ultrasonic_measure(&sensor, MAX_DISTANCE_CM, &distance);
        if (res != ESP_OK)
        {
            printf("Error %d: ", res);
            switch (res)
            {
                case ESP_ERR_ULTRASONIC_PING:
                    printf("Cannot ping (device is in invalid state)\n");
                    break;
                case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                    printf("Ping timeout (no device found)\n");
                    break;
                case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                    printf("Echo timeout (i.e. distance too big)\n");
                    break;
                default:
                    printf("%s\n", esp_err_to_name(res));
            }
        }
        else
            printf("Distance: %0.04f m\n", distance);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}