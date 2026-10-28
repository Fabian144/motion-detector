#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
// #include "driver/i2s_std.h"		// Speaker
// #include "driver/sdmmc_host.h" // SD
// #include "sdmmc_cmd.h"					// SD
// #include "esp_vfs_fat.h"				// SD
#include "lcd1602/lcd1602.h"			// LCD
// #include "driver/i2c_master.h"	// LCD
// #include "ultrasonic.h"				// Ultrasonic
// #include "esp_err.h"						// Ultrasonic
#include "dht.h"									// Hygro

#if defined(CONFIG_TYPE_DHT11)
static const dht_sensor_type_t HYGRO_TYPE = DHT_TYPE_DHT11;
#elif defined(CONFIG_TYPE_AM2301)
static const dht_sensor_type_t HYGRO_TYPE = DHT_TYPE_AM2301;
#elif defined(CONFIG_TYPE_SI7021)
static const dht_sensor_type_t HYGRO_TYPE = DHT_TYPE_SI7021;
#endif

// #define TRIGGER_GPIO CONFIG_TRIGGER_GPIO
// #define ECHO_GPIO CONFIG_ECHO_GPIO
// #define MAX_ULTRASONIC_DISTANCE CONFIG_MAX_DISTANCE
// #define POTENTIOMETER_GPIO CONFIG_POTENTIOMETER_GPIO
// #define BUZZER_GPIO CONFIG_BUZZER_GPIO
#define HYGRO_GPIO CONFIG_HYGRO_GPIO

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
{
	xTaskCreate(print_dht_data, "print_dht_data", 2048, NULL, 5, NULL);
}
