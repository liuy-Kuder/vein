#include "vein.h"
#include "v_i2c.h"

static int try_address(i2c_cmd_handle_t i2c_handle, unsigned char addr, int retries)
{
	int i, ret = 0;

	for (i = 0; i <= retries; i++)
	{
		ret = i2c_master_write_byte(i2c_handle, addr, true);
		if (ret == ESP_OK || i == retries)
		 break;
		i2c_master_stop(i2c_handle);
		i2c_master_start(i2c_handle);
		/*i2c_stop(pdat);
		I2C_DELAY(pdat->udelay);
		i2c_start(pdat);*/
	}

	return ret;
}

static int bit_do_address(i2c_cmd_handle_t i2c_handle, struct i2c_msg_t * msg)
{
	unsigned short flags = msg->flags;
	unsigned short nak_ok = msg->flags & I2C_M_IGNORE_NAK;
	unsigned char addr;
	int ret, retries;
	retries = nak_ok ? 0 : 3;

	if (flags & I2C_M_TEN)
	{
		addr = 0xf0 | ((msg->addr >> 7) & 0x06);

		ret = try_address(i2c_handle, addr, retries);
		if((ret != ESP_OK) && !nak_ok)
		{
			return -1;
		}

		ret = i2c_master_write_byte(i2c_handle, msg->addr & 0xff, true);
		if((ret != ESP_OK) && !nak_ok)
		{
			return -1;
		}

		if(flags & I2C_M_RD)
		{
			i2c_master_start(i2c_handle);
			addr |= 0x01;
			ret = try_address(i2c_handle, addr, retries);
			if((ret != ESP_OK) && !nak_ok)
			{
				return -1;
			}
		}
	}
	else
	{
		addr = msg->addr << 1;
		if(flags & I2C_M_RD)
			addr |= I2C_READ;
		if(flags & I2C_M_REV_DIR_ADDR)
			addr ^= I2C_READ;
		ret = try_address(i2c_handle, addr, retries);
		if((ret != ESP_OK) && !nak_ok)
		  return -1;
	}
	return 0;
}

int i2c_hw_xfer(struct i2c_pdata_t * pdat, struct i2c_msg_t * msgs, int num)
{
	struct i2c_msg_t * pmsg;
	int i, ret;
	unsigned short nak_ok;
	esp_err_t err = ESP_OK;
	i2c_cmd_handle_t i2c_handler = i2c_cmd_link_create();
    ret = i2c_master_start(i2c_handler);
    if (ret != ESP_OK) {
        goto bailout;
    }

	for(i = 0; i < num; i++)
	 {
		pmsg = &msgs[i];
		nak_ok = pmsg->flags & I2C_M_IGNORE_NAK;
		if(!(pmsg->flags & I2C_M_NOSTART))
		 {
			if(i)
			 {
				ret = i2c_master_start(i2c_handler);
				if (ret != ESP_OK) 
				 {
					goto bailout;
				 }
			 }
			err = bit_do_address(i2c_handler, pmsg);
			if (err != ESP_OK) {
				goto bailout;
			}
		 }
		if(pmsg->flags & I2C_M_RD)
		 {
			ret = i2c_master_read(i2c_handler, pmsg->buf, pmsg->len, I2C_MASTER_NACK);
			if (ret != ESP_OK)
			 {
				if (ret >= 0)
					ret = -1;
				goto bailout;
			 }
		 }
		else
		 {
			ret = i2c_master_write(i2c_handler, pmsg->buf, pmsg->len, I2C_MASTER_NACK);
			if (ret != ESP_OK)
			 {
				if (ret >= 0)
				  ret = -1;
				goto bailout;
			 }
		 }
	 }
bailout:
	i2c_master_stop(i2c_handler);
    i2c_master_cmd_begin(pdat->i2c_num, i2c_handler, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(i2c_handler);
	return ret;
}

static int i2c_xfer(struct i2c_t * i2c, struct i2c_msg_t * msgs, int num)
{
	struct i2c_pdata_t * pdat = (struct i2c_pdata_t *)i2c->priv;
	return i2c_hw_xfer(pdat, msgs, num);
}

//i2c探测函数 0 ok , -1 err
static int i2c_probe(struct vein_driver_t * driver, void * table)
{
	struct i2c_pdata_t * i2c_drv = (struct i2c_pdata_t *)table;
	struct i2c_t * i2c;

	i2c = VEIN_MALLOC(sizeof(struct i2c_t));
	if(!i2c)
	 {
		return -1;
	 }

    int i2c_master_port = i2c_drv->i2c_num;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = i2c_drv->i2c_io_sda,
        .scl_io_num = i2c_drv->i2c_io_scl,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = i2c_drv->speed,
    };
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);

    i2c->name = i2c_drv->node_name;
	i2c->xfer = i2c_xfer;
	//i2c->priv = table;
	vein_bus_add_regular(driver->bus, i2c_drv->node_name, i2c);
	return 0;
}

//移除相关内存
static int i2c_remove(struct vein_driver_t * drv,char *node_name)
{
	struct vein_bus_t * i2c_node = NULL;

	if(drv == NULL)
		return -1;
	if(drv->bus == NULL)
		return -2;
	i2c_node = vein_bus_search(drv->bus, node_name);//寻找节点
	VEIN_FREE(i2c_node->priv);//释放私有数据
	return vein_bus_remove_self(i2c_node);
}

static struct vein_driver_t i2c = {
	.name		= "i2c",
	.probe		= i2c_probe,
	.remove		= i2c_remove,
};

int i2c_driver_init(void)
{
	return vein_register_driver(&i2c);
}

int i2c_driver_exit(void)
{
	return vein_unregister_driver(&i2c);
}

struct i2c_pdata_t i2c_drv_info[] = {
	{
        .node_name = "i2c0",
		.i2c_num = 0,
	    .i2c_io_scl = 0,
		.i2c_io_sda = 1,
    	.speed = 400,
		.timeout = 100,
    },
	{
        .node_name = "i2c1",
       // .i2cHandle = &hi2c1,
		.timeout = 100,
    },
};

void init_i2c_node(void)
{
	int i;
	struct vein_driver_t * drv_t;
	drv_t = vein_search_driver("i2c");
	if(drv_t != NULL)
	 {
		for(i = 0;i < ARRAY_SIZE(i2c_drv_info); i++)
		drv_t->probe(drv_t,&i2c_drv_info[i]);
	 }
	//drv_t->remove(drv_t,&drv);//移除
}
