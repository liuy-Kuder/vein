#include "vein.h"
#include "driver_gpio.h"
#include "driver/gpio.h"

static void pwm_io_set_cfg(struct pwm_gpio_t* pwm_io, uint8_t polarity, uint32_t freq, uint8_t duty)
{

}

static int pwm_io_enable(struct pwm_gpio_t* pwm_io)
{
	pwm_io->isEnable = 1;
}

static int pwm_io_disable(struct pwm_gpio_t* pwm_io)
{
	pwm_io->isEnable = 0;
}

static void pwm_io_set_freq(struct pwm_gpio_t* pwm_io, uint32_t freq)
{
	pwm_io->freq = freq;
}

static void pwm_io_set_polarity_duty(struct pwm_gpio_t* pwm_io, uint8_t polarity, uint8_t duty)
{
	pwm_io->polarity = polarity;
	pwm_io->duty = duty;
}

static int pwm_io_get_param(struct pwm_gpio_t* pwm_io, uint16_t pin)
{
    return 0;
}

static int pwm_io_get_max_freq(struct pwm_gpio_t* pwm_io, uint16_t pin)
{
	return 1000000 / pwm_io->timer_us;
}

static int16_t pwm_gpio_probe(struct vein_driver_t * driver, void * table)
{
	struct gpio_pdata_t * gpio_drv = (struct gpio_pdata_t *)table;
	struct pwm_gpio_t* pwm_io;

	pwm_io = VEIN_MALLOC(sizeof(struct gpio_t));
	if(!pwm_io)
	 {
		return -1;
	 }

	pwm_io->name = "bus";
	pwm_io->set_cfg = io_set_cfg;
	pwm_io->set_value = io_set_value;
	pwm_io->get_value = io_get_value;
	pwm_io->enable_irq = io_enable_irq;
	pwm_io->disable_irq = io_disable_irq;
	pwm_io->priv = NULL;
	vein_bus_add_regular(driver->bus, gpio_drv->node_name, pwm_io);
	return 0;
}

//移除相关内存
static int16_t pwm_gpio_remove(struct vein_driver_t * drv, char *node_name)
{
	struct vein_bus_t * i2c_node = NULL;

	if(drv == NULL)
		return -1;
	if(drv->bus == NULL)
		return -2;
	i2c_node = vein_bus_search(drv->bus, "bus");//寻找节点
	VEIN_FREE(i2c_node->priv);//释放私有数据
	return vein_bus_remove_self(i2c_node);
}

static struct vein_driver_t pwm_gpio = {
	.name		= "pwm_gpio",
	.probe		= gpio_probe,
	.remove		= gpio_remove,
};

int pwm_gpio_driver_init(void)
{
	return vein_register_driver(&pwm_gpio);
}

int pwm_gpio_driver_exit(void)
{
	return vein_unregister_driver(&pwm_gpio);
}

void init_pwm_gpio_node(void)
{
	struct gpio_pdata_t gpio_init;
	gpio_init.node_name = "pwm0";

	struct vein_driver_t * drv_t;
	drv_t = vein_search_driver("pwm_gpio");
	if(drv_t != NULL)
	 {
		drv_t->probe(drv_t,&gpio_init);
	 }
}

int gpio_set_cfg(void * group, uint16_t pin, enum gpio_direction_t dir, enum gpio_pull_t pull, enum gpio_rate_t rate)
{
	struct vein_driver_t * driver = vein_search_driver("gpio");
	if(driver != NULL)
     {
        struct vein_bus_t * bus = vein_bus_search(driver->bus, "gpion");//寻找节点
		struct gpio_t * gpio = (struct gpio_t *)bus->priv;
		gpio->set_cfg(gpio, group, pin, dir, pull, rate);
        return 0;
     }
	return -1;
}

int gpio_set_value(void * group, uint16_t pin, uint8_t value)
{
	struct vein_driver_t * driver = vein_search_driver("gpio");
	if(driver != NULL)
     {
        struct vein_bus_t * bus = vein_bus_search(driver->bus, "gpion");//寻找节点
		struct gpio_t * gpio = (struct gpio_t *)bus->priv;
		gpio->set_value(gpio, group, pin, value);
        return 0;
     }
	return -1;
}

int gpio_get_value(void * group, uint16_t pin)
{
	struct vein_driver_t * driver = vein_search_driver("gpio");
	if(driver != NULL)
     {
        struct vein_bus_t * bus = vein_bus_search(driver->bus, "gpion");//寻找节点
		struct gpio_t * gpio = (struct gpio_t *)bus->priv;
		return gpio->get_value(gpio, group, pin);
     }
	return -1;
}

int gpio_enable_irq(void * group, uint16_t pin,v_gpio_irq_handler_t irq_handler, enum gpio_interrput_t intr_type)
{
	struct vein_driver_t * driver = vein_search_driver("gpio");
	if(driver != NULL)
     {
        struct vein_bus_t * bus = vein_bus_search(driver->bus, "gpion");//寻找节点
		struct gpio_t * gpio = (struct gpio_t *)bus->priv;
		return gpio->enable_irq(gpio, group, pin, irq_handler, intr_type);
     }
	return -1;
}

int gpio_disable_irq(void * group, uint16_t pin)
{
	struct vein_driver_t * driver = vein_search_driver("gpio");
	if(driver != NULL)
     {
        struct vein_bus_t * bus = vein_bus_search(driver->bus, "gpion");//寻找节点
		struct gpio_t * gpio = (struct gpio_t *)bus->priv;
		return gpio->disable_irq(gpio, group, pin);
     }
	return -1;
}

static int pwm_io_process(void)
{


	return 0;
}