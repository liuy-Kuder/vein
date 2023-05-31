#ifndef  __DRIVER_GPIO_H_
#define  __DRIVER_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "vein.h"

typedef void(*v_gpio_irq_handler_t)(void *arg);

struct pwm_gpio_t
{
	char * name;
	uint8_t isEnable;
	uint8_t polarity;
	uint8_t duty;
	uint32_t freq;
	uint32_t timer_us;//仅支持微秒级定时
	
	void (*set_cfg)(struct pwm_gpio_t* pwm_io, uint8_t polarity, uint32_t freq, uint8_t duty);
	void (*set_dir)(struct pwm_gpio_t* gpio, int offset, enum gpio_direction_t dir);
	enum gpio_direction_t (*get_dir)(struct pwm_gpio_t* gpio, int offset);
	void (*set_value)(struct pwm_gpio_t* gpio, uint16_t pin, uint8_t value);
	int  (*get_value)(struct pwm_gpio_t* gpio, uint16_t pin);
	int (*enable_irq)(struct pwm_gpio_t* gpio, uint16_t pin, v_gpio_irq_handler_t irq_handler, enum gpio_interrput_t intr_type);
	int (*disable_irq)(struct pwm_gpio_t* gpio, uint16_t pin);
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
