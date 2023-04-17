#include "vein.h"
#include "driver_gpio.h"
#include "driver/gpio.h"

static void io_set_cfg(struct gpio_t * gpio, void * group, uint16_t pin, \
                         enum gpio_direction_t dir, enum gpio_pull_t pull, enum gpio_rate_t rate)
{
    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //disable interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.pin_bit_mask = 1ULL << pin;
	switch(dir)
	 {
		case GPIO_DIRECTION_INPUT:
			io_conf.mode = GPIO_MODE_INPUT;
			break;
		case GPIO_DIRECTION_OUTPUT_PP:
			io_conf.mode = GPIO_MODE_OUTPUT;
			break;
		case GPIO_DIRECTION_OUTPUT_OD:
			io_conf.mode = GPIO_MODE_OUTPUT_OD;
			break;
		default:break;
	 }
	switch(pull)
	 {
		case GPIO_PULL_UP:
			io_conf.pull_down_en = 0;
			io_conf.pull_up_en = 1;
			break;
		case GPIO_PULL_DOWN:
			io_conf.pull_down_en = 1;
			io_conf.pull_up_en = 0;
			break;
		case GPIO_PULL_NONE:
			io_conf.pull_down_en = 0;
			io_conf.pull_up_en = 0;
			break;
		default:break;	
	 }
    gpio_config(&io_conf);
	gpio_set_drive_capability(pin,rate);
}

static int io_enable_irq(struct gpio_t * gpio, void * group, uint16_t pin, \
						   v_gpio_irq_handler_t irq_handler, enum gpio_interrput_t intr_type)
{
	gpio_install_isr_service(0);
	gpio_intr_enable(pin);
	gpio_set_intr_type(pin, intr_type);
	return gpio_isr_handler_add(pin, irq_handler, pin);
}

static int io_disable_irq(struct gpio_t * gpio, void * group, uint16_t pin)
{
	gpio_intr_disable(pin);
	gpio_set_intr_type(pin, 0);
	return gpio_isr_handler_remove(pin);
}

static void io_set_value(struct gpio_t * gpio, void * group, uint16_t pin, uint8_t value)
{
    gpio_set_level(pin, value);
}

static int io_get_value(struct gpio_t * gpio, void * group, uint16_t pin)
{
    return gpio_get_level(pin);
}

static int16_t gpio_probe(struct vein_driver_t * driver, void * table)
{
	struct gpio_pdata_t * gpio_drv = (struct gpio_pdata_t *)table;
	struct gpio_t * gpio;

	gpio = VEIN_MALLOC(sizeof(struct gpio_t));
	if(!gpio)
	 {
		return -1;
	 }
	gpio->base = 0;
	gpio->ngpio = 39;
    gpio->name = "bus";
	gpio->set_cfg = io_set_cfg;
    gpio->set_value = io_set_value;
    gpio->get_value = io_get_value;
	gpio->enable_irq = io_enable_irq;
	gpio->disable_irq = io_disable_irq;
	gpio->priv = NULL;
	vein_bus_add_regular(driver->bus, gpio_drv->node_name, gpio);
	return 0;
}

//移除相关内存
static int16_t gpio_remove(struct vein_driver_t * drv, char *node_name)
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

static struct vein_driver_t gpio = {
	.name		= "gpio",
	.probe		= gpio_probe,
	.remove		= gpio_remove,
};

int gpio_driver_init(void)
{
	return vein_register_driver(&gpio);
}

int gpio_driver_exit(void)
{
	return vein_unregister_driver(&gpio);
}

void init_gpio_node(void)
{
	struct gpio_pdata_t gpio_init;
	gpio_init.node_name = "gpion";

	struct vein_driver_t * drv_t;
	drv_t = vein_search_driver("gpio");
	if(drv_t != NULL)
	 {
		drv_t->probe(drv_t,&gpio_init);
	 }
	//drv_t->remove(drv_t,&drv);//移除
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

