#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
// #include "driver/i2s_std.h"		// Speaker
// #include "driver/sdmmc_host.h" // SD
// #include "sdmmc_cmd.h"					// SD
// #include "esp_vfs_fat.h"				// SD
#include "lcd1602/lcd1602.h" // LCD
// #include "driver/i2c_master.h"	// LCD
// #include "ultrasonic.h"				// Ultrasonic
// #include "esp_err.h"						// Ultrasonic
#include "dht.h" // Hygro

#if defined(CONFIG_TYPE_DHT11)
static const dht_sensor_type_t HYGRO_TYPE = DHT_TYPE_DHT11;
#elif defined(CONFIG_TYPE_AM2301)
static const dht_sensor_type_t HYGRO_TYPE = DHT_TYPE_AM2301;
#elif defined(CONFIG_TYPE_SI7021)
static const dht_sensor_type_t HYGRO_TYPE = DHT_TYPE_SI7021;
#endif

// #define TRIGGER_GPIO 						CONFIG_TRIGGER_GPIO
// #define ECHO_GPIO 								CONFIG_ECHO_GPIO
// #define MAX_ULTRASONIC_DISTANCE 	CONFIG_MAX_DISTANCE
// #define POTENTIOMETER_GPIO 			CONFIG_POTENTIOMETER_GPIO
// #define BUZZER_GPIO 							CONFIG_BUZZER_GPIO
#define HYGRO_GPIO CONFIG_HYGRO_GPIO
#define ESP_I2C_PORT CONFIG_ESP_I2C_PORT
#define ESP_I2C_SDA CONFIG_ESP_I2C_SDA
#define ESP_I2C_SCL CONFIG_ESP_I2C_SCL
#define ESP_I2C_ADDRESS LCD1602_I2C_ADDRESS_DEFAULT

float temperature, humidity;

void set_dht_data(void *pvParameters);
void display_dht_data(void *pvParameters);

void app_main()
{
	xTaskCreate(set_dht_data, "set dht data", 2048, NULL, 4, NULL);
	xTaskCreate(display_dht_data, "show dht data on the lcd display", 4096, NULL, 5, NULL);
}

void set_dht_data(void *pvParameters)
{
#ifdef CONFIG_INTERNAL_PULLUP
	gpio_set_pull_mode(HYGRO_GPIO, GPIO_PULLUP_ONLY);
#endif

	while (1)
	{
		if (dht_read_float_data(HYGRO_TYPE, HYGRO_GPIO, &humidity, &temperature) != ESP_OK)
		{
			printf("Could not read data from sensor\n");
		}

		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void display_dht_data(void *pvParameters)
{
	i2c_lowlevel_config config = {0};
	config.port = ESP_I2C_PORT;
	config.pin_sda = ESP_I2C_SDA;
	config.pin_scl = ESP_I2C_SCL;

	lcd1602_context *ctx = lcd1602_init(ESP_I2C_ADDRESS, true, &config);
	if (ctx == NULL)
	{
		printf("Could not initialize LCD display\n");
		vTaskDelete(NULL);
	}

	char line[32];

	while (1)
	{
		snprintf(line, sizeof line, "Temp: %.1fC", temperature);
		lcd1602_set_cursor(ctx, 0, 0);
		lcd1602_string(ctx, line);

		snprintf(line, sizeof line, "Hum:  %.1f%%", humidity);
		lcd1602_set_cursor(ctx, 1, 0);
		lcd1602_string(ctx, line);

		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}