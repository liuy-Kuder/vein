/* SPI Master Half Duplex EEPROM example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "spi_eeprom.h"
#include "vein.h"
#include "v_driver.h"
#include "v_device.h"

static const char TAG[] = "main";
#define BULE_LED_PIN     2
#define USER_KEY_PIN     0

int gpio_irq = 1;

static void gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    gpio_irq = gpio_get_level(gpio_num);
}

void app_main(void)
{
    esp_err_t ret;
    vein_init();
    ESP_LOGI(TAG, "vein init finished.");
    gpio_driver_init();
    init_gpio_node();

    gpio_set_cfg(NULL, BULE_LED_PIN, GPIO_DIRECTION_OUTPUT_PP, GPIO_PULL_UP, GPIO_RATE_HIGH);
    gpio_set_cfg(NULL, USER_KEY_PIN, GPIO_DIRECTION_INPUT, GPIO_PULL_UP, GPIO_RATE_HIGH);
    gpio_set_value(NULL, BULE_LED_PIN, 0);
    gpio_enable_irq(NULL, USER_KEY_PIN, gpio_isr_handler, GPIO_INT_FALLING_EDGE);
    vTaskDelay(500 / portTICK_RATE_MS);
    while (1) {
        // Add your main loop handling code here.
        gpio_set_value(NULL, BULE_LED_PIN, 1);
        vTaskDelay(500 / portTICK_RATE_MS);
        gpio_set_value(NULL, BULE_LED_PIN, 0);
        vTaskDelay(500 / portTICK_RATE_MS);
     }
}
