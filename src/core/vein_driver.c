/********************************************************************
*
*文件名称：vein_driver.c
*内容摘要：提供驱动管理平台
*当前版本：V1.0
*作者：刘杨
*完成时期：2022.12.23
*其他说明: none
*
**********************************************************************/
#include "vein.h"

static struct hlist_head __vein_driver_hash[DRIVER_HASH_SIZE];

static struct hlist_head * vein_driver_hash(const char * name)
{
	return &__vein_driver_hash[shash(name) % ARRAY_SIZE(__vein_driver_hash)];
}

/********************************************************************
*                      功能函数
*功能描述：寻找驱动kobj
*输入参数：无
*返回值：struct kobj_t
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.12.23     1.0        刘杨
**********************************************************************/
static struct vein_bus_t * vein_search_driver_root(void)
{
	return vein_bus_search_directory_with_create(vein_bus_get_root(), "class");
}

/********************************************************************
*                      功能函数
*功能描述：根据名字寻找驱动
*输入参数：无
*返回值：驱动地址
*其他说明：NULL or POS
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.12.23     1.0        刘杨
**********************************************************************/
struct vein_driver_t * vein_search_driver(const char * name)
{
	struct vein_driver_t * pos;
	struct hlist_node * n;

	if(!name)
	  return NULL;

    hlist_for_each_entry_safe(pos, struct vein_driver_t, n, vein_driver_hash(name), node)
	 {
		if(strcmp(pos->name, name) == 0)
		  return pos;
	 }
	return NULL;
}

/********************************************************************
*                      功能函数
*功能描述：根据驱动名字寻找总线
*输入参数：无
*返回值：总线地址
*其他说明：NULL or bus_t
*修改日期        版本      修改人        修改内容
*---------------------------------------------------------------------
*2023.2.4         1.0      刘杨
**********************************************************************/
struct vein_bus_t * vein_search_driver_bus(const char * driver_name, const char * bus_name)
{
    struct vein_driver_t * driver = vein_search_driver(driver_name);
	if(driver != NULL)
     {
        struct vein_bus_t * bus = vein_bus_search(driver->bus, bus_name);//寻找节点
        return bus;
     }
	return NULL;
}

/********************************************************************
*                      功能函数
*功能描述：注册驱动
*输入参数：无
*返回值：false：失败 true：成功
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.12.23     1.0        刘杨
**********************************************************************/
int vein_register_driver(struct vein_driver_t * driver)
{
	if(!driver || !driver->name)//空或无名字、返回假
	 {
		VEIN_LOG_ERROR("%s register driver null or name null!\n",driver->name);
		return VEIN_ERR;
	 }

	if(!driver->probe || !driver->remove)//无管道或无移除、返回假
	 {
		VEIN_LOG_ERROR("%s register driver no probe or remove!\n",driver->name);
		return VEIN_ERR;
	 }

	if(vein_search_driver(driver->name))//寻找驱动名，如果相同则返回假
	 {
		VEIN_LOG_ERROR("%s register driver have same name!\n",driver->name);
		return VEIN_ERR;
	 }

	driver->bus = vein_bus_alloc_directory(driver->name);
	vein_bus_add(vein_search_driver_root(), driver->bus);
	spin_lock_irq();//关中断
	init_hlist_node(&driver->node);
	hlist_add_head(&driver->node, vein_driver_hash(driver->name));
	spin_unlock_irq();//开中断
	VEIN_LOG_TRACE("%s register driver success!\n",driver->name);
	return VEIN_OK;
}

/********************************************************************
*                      功能函数
*功能描述：卸载驱动
*输入参数：无
*返回值：false：失败 true：成功
*其他说明：无
*修改日期          版本         修改人        修改内容
*---------------------------------------------------------------------
*2022.12.23     1.0        刘杨
**********************************************************************/
int vein_unregister_driver(struct vein_driver_t * driver)
{
	if(!driver || !driver->name)
	 {
		VEIN_LOG_ERROR("%s unregister driver null or name null!\n",driver->name);
		return VEIN_ERR;
	 }

	if(hlist_unhashed(&driver->node))
	 {
		VEIN_LOG_ERROR("%s unregister driver node is null!\n",driver->name);
		return VEIN_ERR;
	 }

	spin_lock_irq();
	hlist_del(&driver->node);
	spin_unlock_irq();
	vein_bus_remove_self(driver->bus);
	VEIN_LOG_TRACE("%s unregister driver success!\n",driver->name);
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
*2022.12.23     1.0        刘杨
**********************************************************************/
void vein_driver_pure_init(void)
{
	unsigned int i;
	for(i = 0; i < ARRAY_SIZE(__vein_driver_hash); i++)
	 init_hlist_head(&__vein_driver_hash[i]);
}
