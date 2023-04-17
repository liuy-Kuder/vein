/********************************************************************
*
*文件名称：vein_device.c
*内容摘要：提供设备管理平台
*当前版本：V1.0
*作者：刘杨
*完成时期：2022.12.27
*其他说明: none
*
**********************************************************************/
#include "vein_device.h"

static struct hlist_head __vein_device_hash[DEVICE_HASH_SIZE];

static struct hlist_head * vein_device_hash(const char * name)
{
	return &__vein_device_hash[shash(name) % ARRAY_SIZE(__vein_device_hash)];
}

/********************************************************************
*                      功能函数
*功能描述：驱动是否存在
*输入参数：名字
*返回值：存在返回true,否则返回false
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
static int vein_device_exist(const char * name)
{
	struct vein_device_t * pos;
	struct hlist_node * n;

	hlist_for_each_entry_safe(pos, struct vein_device_t, n, vein_device_hash(name), node)
	 {
		if(strcmp(pos->name, name) == 0)
		  return VEIN_OK;
	 }
	return VEIN_ERR;
}

/********************************************************************
*                      功能函数
*功能描述：自动分配驱动名称,如果当前设备的ID已被注册,则自动增加,重新分配
*输入参数：设备名字,ID
*返回值：返回分配好的名称地址
*其他说明：需要free释放这个返回值
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
char * vein_alloc_device_name(const char * name, uint16_t id)
{
	char buf[DEVICE_NAME_SIZE] = {0};

	do {
		snprintf(buf, sizeof(buf), "%s.%d", name, id++);
	} while(vein_device_exist(buf) == VEIN_OK);

	return strdup(buf);
}

/********************************************************************
*                      功能函数
*功能描述：释放设备的名称
*输入参数：名字
*返回值：FALSE or TRUE
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
*2022.12.17    1.1        刘杨         修改返回值
**********************************************************************/
int vein_free_device_name(char * name)
{
	if(name)
	 {
		VEIN_FREE(name);
		return VEIN_OK;
	 }
	else
	 return VEIN_ERR;
}

/********************************************************************
*                      功能函数
*功能描述：寻找设备
*输入参数：name: 名字
*         id: 设备唯一ID
*         type: 设备类型
*返回值：设备地址
*其他说明：无
*修改日期           版本       修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12      1.0       刘杨
*2022.12.27     1.1       刘杨        取消fs后,进行device优化,添加设备ID
**********************************************************************/
struct vein_device_t * vein_search_device(const char * name, uint16_t id)
{
	struct vein_device_t * pos;
	struct hlist_node * n;

	if(!name)
	  return NULL;

    char buf[DEVICE_NAME_SIZE] = {0};
    snprintf(buf, sizeof(buf), "%s.%d", name, id);

	hlist_for_each_entry_safe(pos, struct vein_device_t, n, vein_device_hash(buf), node)
	 {
		if(strcmp(pos->name, buf) == 0)
		  return pos;
	 }
	return NULL;
}

/********************************************************************
*                      功能函数
*功能描述：添加设备接口
*输入参数：device: 具体的设备参数
*          read : 读接口
*          write : 写接口
*          ioctl : 操作接口
*返回值：无
*其他说明：无
*修改日期            版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.12.27      1.0       刘杨
**********************************************************************/
void vein_device_init_basic_if(struct vein_device_t * device, vein_device_read_t read, vein_device_write_t write, vein_device_ioctl_t ioctl)
{
    device->read = read;
    device->write = write;
    device->ioctl = ioctl;
}

/********************************************************************
*                      功能函数
*功能描述：添加设备休眠操作
*输入参数：device : 具体的设备参数
*          shutdown : 设备关机
*          startup : 设备开机
*返回值：无
*其他说明：无
*修改日期            版本       修改人        修改内容
*---------------------------------------------------------------------
*2022.12.27      1.0       刘杨
**********************************************************************/
void vein_device_init_low_power_if(struct vein_device_t * device, vein_device_shutdown_t shutdown, vein_device_startup_t startup)
{
#ifdef DEVICE_INIT_LOW_POWER_IF
    device->shutdown = shutdown;
    device->startup = startup;
#endif
}

/********************************************************************
*                      功能函数
*功能描述：添加设备暂停操作
*输入参数：device : 具体的设备参数
*          suspend : 设备挂起接口
*          resume : 设备恢复接口
*返回值：无
*其他说明：无
*修改日期            版本       修改人        修改内容
*---------------------------------------------------------------------
*2022.12.27      1.0       刘杨
**********************************************************************/
void vein_device_init_pause_if(struct vein_device_t * device, vein_device_suspend_t suspend, vein_device_resume_t resume)
{
#ifdef DEVICE_INIT_PAUSE_IF
    device->suspend = suspend;
    device->resume = resume;
#endif
}

/********************************************************************
*                      功能函数
*功能描述：device数量累加
*输入参数：无
*返回值：无
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.10.6     1.0        刘杨
**********************************************************************/
static uint16_t vein_decive_cnt = 0;
static void vein_device_inc(void)
{
	vein_decive_cnt++;
}

/********************************************************************
*                      功能函数
*功能描述：device数量减少
*输入参数：无
*返回值：无
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.10.6     1.0        刘杨
**********************************************************************/
static void vein_device_dec(void)
{
	vein_decive_cnt--;
}

/********************************************************************
*                      功能函数
*功能描述：获得device的数量
*输入参数：无
*返回值：device数量
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.10.6     1.0        刘杨
**********************************************************************/
uint16_t vein_get_register_device_num(void)
{
	return vein_decive_cnt;
}

/********************************************************************
*                      功能函数
*功能描述：注册设备
*输入参数：vein_device_t
*返回值：成功返回true,失败返回false
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12      1.0       刘杨
*2022.12.27     1.1       刘杨   取消fs后,进行device优化
**********************************************************************/
int vein_register_device(struct vein_device_t * dev)
{
	if(!dev || !dev->name)
	 {
		VEIN_LOG_ERROR("%s register device null or name null!\n",dev->name);
		return VEIN_ERR;
	 }

    dev->name = vein_alloc_device_name(dev->name, dev->id);
	if(vein_device_exist(dev->name) != VEIN_OK)
	 {
	    VEIN_FREE(dev->name);
		VEIN_LOG_ERROR("%s register device have existing!\n",dev->name);
		return VEIN_ERR;
	 }

	spin_lock_irq();
	init_hlist_node(&dev->node);
	hlist_add_head(&dev->node, vein_device_hash(dev->name));
	spin_unlock_irq();
	VEIN_LOG_TRACE("%s device install success!\n",dev->name);
	vein_device_inc();
	return VEIN_OK;
}

/********************************************************************
*                      功能函数
*功能描述：卸载设备
*输入参数：vein_device_t
*返回值：成功返回true,失败返回false
*其他说明：无
*修改日期           版本       修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12      1.0       刘杨
*2022.12.27     1.1       刘杨        取消fs后,进行device优化
**********************************************************************/
int vein_unregister_device(struct vein_device_t * dev)
{
	if(!dev || !dev->name)
	 {
		VEIN_LOG_ERROR("%s unregister device null or name null!\n",dev->name);
		return VEIN_ERR;
	 }

	if(hlist_unhashed(&dev->node))
	 {
		VEIN_LOG_ERROR("%s unregister device fail!\n",dev->name);
		return VEIN_ERR;
	 }

	spin_lock_irq();
	hlist_del(&dev->node);
	spin_unlock_irq();
	VEIN_LOG_TRACE("%s unregister device success!\n",dev->name);
	vein_device_dec();
	return VEIN_OK;
}

/********************************************************************
*                      功能函数
*功能描述：初始化哈希池
*输入参数：无
*返回值：无
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12      1.0       刘杨
*2022.12.27     1.1       刘杨        取消fs后,进行device优化
**********************************************************************/
void vein_device_pure_init(void)
{
	uint16_t i;
	for(i = 0; i < ARRAY_SIZE(__vein_device_hash); i++)
	  init_hlist_head(&__vein_device_hash[i]);
}
