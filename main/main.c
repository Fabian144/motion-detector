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

void app_main(void) { }
