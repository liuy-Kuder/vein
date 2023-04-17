#include "at24cxx.h"
#include "vein.h"
#include "v_driver.h"

/*
offset: 寄存器位置偏移
buf: 缓冲
size: 数量
*/
static int16_t at24cxx_read(struct vein_device_t * device, uint32_t offset, void * buf, uint32_t size)
{
	struct at24cxx_device_priv_t * at24cxx = (struct at24cxx_device_priv_t *)device->priv;
	struct i2c_t * i2c = (struct i2c_t *)device->bus->priv;
	struct i2c_msg_t msgs[2];
	msgs[0].addr = at24cxx->addr;//寄存器地址
	msgs[0].flags = 0;//写指令
	msgs[0].buf = &offset;
	msgs[0].len = 2;//长度

	msgs[1].addr = at24cxx->addr;//寄存器地址
	msgs[1].flags = I2C_M_RD;//读指令
	msgs[1].buf = buf;
	msgs[1].len = size;//长度
	return i2c->xfer(i2c, msgs, 2);
}

/*
offset: 寄存器位置偏移
buf: 缓冲
size: 数量
*/
static int16_t at24cxx_write(struct vein_device_t * device, uint32_t offset, void * buf, uint32_t size)
{
	struct at24cxx_device_priv_t * at24cxx = (struct at24cxx_device_priv_t *)device->priv;
	struct i2c_t * i2c = (struct i2c_t *)device->bus->priv;
	struct i2c_msg_t msgs[2];
    msgs[0].addr = at24cxx->addr;//寄存器地址
	msgs[0].flags = 0;//写指令
	msgs[0].buf = &offset;
	msgs[0].len = 2;//长度

	msgs[1].addr = at24cxx->addr;//寄存器地址
	msgs[1].flags = I2C_M_RD;//读指令
	msgs[1].buf = buf;
	msgs[1].len = size;//长度
    return i2c->xfer(i2c, msgs, 2);
}

//写保护
static int16_t at24cxx_ioctl(struct vein_device_t * device, uint64_t cmd, ...)
{
	struct at24cxx_device_priv_t * at24cxx = (struct at24cxx_device_priv_t *)device->priv;
	if(at24cxx->wp_ctrl == NULL)
	 return 0;
    switch(cmd)
      {
		case AT24CXX_IOCTL_WP_ON:
			at24cxx->wp_ctrl(0);
			break;
		case AT24CXX_IOCTL_WP_OFF:
			at24cxx->wp_ctrl(1);
			break;
        default:break;
      }
    return 0;
}

//注册at24cxx设备
struct vein_device_t * register_at24cxx_dev(struct device_at24cxx_info_t * table)
{
	struct vein_device_t * at24cxx = NULL;
	struct at24cxx_device_priv_t * at24cxx_priv = NULL;
	
	at24cxx = VEIN_MALLOC(sizeof(struct vein_device_t));
	if(!at24cxx)
	  return NULL;

	at24cxx_priv = VEIN_MALLOC(sizeof(struct at24cxx_device_priv_t));
	if(!at24cxx_priv)
	  return NULL;

    at24cxx->name = table->base_info.device_name;
	at24cxx->id = table->base_info.id;
    at24cxx->bus = vein_search_driver_bus(table->base_info.driver_name, table->base_info.bus_name);
    if(at24cxx->bus == NULL)
      return NULL;
    memcpy(at24cxx_priv, &table->priv_info, sizeof(struct at24cxx_device_priv_t));
	at24cxx->priv = at24cxx_priv;
	vein_device_init_basic_if(at24cxx, at24cxx_read, at24cxx_write, at24cxx_ioctl);
	if(!vein_register_device(at24cxx))
	 {
		VEIN_FREE(at24cxx->priv);
		VEIN_FREE(at24cxx);
		return NULL;
	 }
	return at24cxx;
}

//卸载驱动
//参数驱动名name
int16_t unregister_at24cxx_dev(char *name, uint16_t id)
{
	struct vein_device_t * dev;
	dev = vein_search_device(name, id);
	if(dev && vein_unregister_device(dev))
	 {
		vein_free_device_name(dev->name);
		VEIN_FREE(dev);
		return VEIN_OK;
	 }
	return VEIN_ERR;
}

void at24cxx_wp_pin(uint8_t state)
{
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

struct vein_device_t * init_at24cxx_device(void)
{
    struct device_at24cxx_info_t at24cxx_dev_info[] = {
        {
          .base_info = {
            .driver_name = "i2c",
            .bus_name = "i2c0",
            .device_name = "at24c02",
            .id = 0,
          },
          .priv_info = {
            .addr = 0x50,
            .page = 32,
            .page_size = 8,
            .wp_ctrl = at24cxx_wp_pin,
          }
        },
        {
          .base_info = {
            .driver_name = "i2c",
            .bus_name = "i2c0",
            .device_name = "at24c02",
            .id = 0,
          },
          .priv_info = {
            .addr = 0x50,
            .page = 32,
            .page_size = 8,
            .wp_ctrl = at24cxx_wp_pin,
          }
        }
    };
    return register_at24cxx_dev(&at24cxx_dev_info[0]);
}