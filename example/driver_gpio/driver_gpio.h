#ifndef  __DRIVER_GPIO_H_
#define  __DRIVER_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "vein.h"

enum gpio_pull_t {
	GPIO_PULL_UP			= 0,
	GPIO_PULL_DOWN			= 1,
	GPIO_PULL_NONE			= 2,
};

enum gpio_rate_t {
	GPIO_RATE_LOW			= 0,
	GPIO_RATE_MEDIUM		= 1,
    GPIO_RATE_HIGH			= 2,
	GPIO_RATE_FAST			= 3,
};

enum gpio_direction_t {
	GPIO_DIRECTION_INPUT	 = 0,
	GPIO_DIRECTION_OUTPUT_PP = 1,
    GPIO_DIRECTION_OUTPUT_OD = 2,
};

enum gpio_interrput_t {
	GPIO_INT_DISABLE = 0,		/*!< Disable GPIO interrupt                             */
	GPIO_INT_RISING_EDGE = 1,	/*!< GPIO interrupt type : rising edge                  */
    GPIO_INT_FALLING_EDGE = 2,	/*!< GPIO interrupt type : falling edge                 */
	GPIO_INT_ANYEDGE = 3, 		/*!< GPIO interrupt type : both rising and falling edge */
	GPIO_INT_LOW_LEVEL = 4, 	/*!< GPIO interrupt type : input low level trigger      */
	GPIO_INT_HIGH_LEVEL = 5,	/*!< GPIO interrupt type : input high level trigger     */
};

typedef void(*v_gpio_irq_handler_t)(void *arg);

struct gpio_t
{
	char * name;
	int base;
	int ngpio;
	void (*set_cfg)(struct gpio_t * gpio, void * group, uint16_t pin, \
                    enum gpio_direction_t dir, enum gpio_pull_t pull, enum gpio_rate_t rate);
	void (*set_dir)(struct gpio_t * gpio, int offset, enum gpio_direction_t dir);
	enum gpio_direction_t (*get_dir)(struct gpio_t * gpio, int offset);
	void (*set_value)(struct gpio_t * gpio, void * group, uint16_t pin, uint8_t value);
	int  (*get_value)(struct gpio_t * gpio, void * group, uint16_t pin);
	int (*enable_irq)(struct gpio_t * gpio, void * group, uint16_t pin, v_gpio_irq_handler_t irq_handler, enum gpio_interrput_t intr_type);
	int (*disable_irq)(struct gpio_t * gpio, void * group, uint16_t pin);
	void * priv;
};

struct gpio_pdata_t {
	char* node_name;
	uint32_t pin;
	void *group;
};

int gpio_driver_init(void);
int gpio_driver_exit(void);
void init_gpio_node(void);

int gpio_set_cfg(void * group, uint16_t pin, enum gpio_direction_t dir, enum gpio_pull_t pull, enum gpio_rate_t rate);
int gpio_set_value(void * group, uint16_t pin, uint8_t value);
int gpio_get_value(void * group, uint16_t pin);
int gpio_enable_irq(void * group, uint16_t pin,v_gpio_irq_handler_t irq_handler, enum gpio_interrput_t intr_type);
int gpio_disable_irq(void * group, uint16_t pin);

#ifdef __cplusplus
}
#endif

#endif
