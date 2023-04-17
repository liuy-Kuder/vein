/********************************************************************
*
*文件名称：bus.c
*内容摘要：MicroKernel核心功能
*当前版本：V1.0
*作者：刘杨
*完成时期：2022.09.12
*其他说明: none
*
**********************************************************************/
#include <stdlib.h>
#include <string.h>
#include "vein.h"
static struct vein_bus_t * __vein_bus_root = NULL;

/********************************************************************
*                      功能函数
*功能描述：分配一段内存,然后用传递的参数进行初始化,并返回分配的bus节点
*输入参数：
*       name: 名称
*       type: 文件 或 节点
*       priv: 私有数据块
*返回值：分配的bus节点
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
static struct vein_bus_t * __vein_bus_alloc(const char * name, enum vein_bus_type_t type, void * priv)
{
	struct vein_bus_t * bus;

	if(!name)
	 {
		VEIN_LOG_TRACE("name is null!\n");
		return NULL;//名称不为空
	 }
	bus = VEIN_MALLOC(sizeof(struct vein_bus_t));//分配字节内存
	if(!bus)
	 {
	    VEIN_LOG_TRACE("%s malloc fail!\n",name);
	    return NULL;//分配失败
	 }

	bus->name = strdup(name);//拷贝
	bus->type = type;//节点属性
	bus->parent = bus;//父类
	bus->priv = priv;//私有属性

	init_list_head(&bus->entry);//初始化列表
	init_list_head(&bus->children);
	return bus;
}
/********************************************************************
*                      功能函数
*功能描述：在获取根节点时，如果不存在，则会自动创建一个名为bus的根节点，
*		  该节点为全局静态变量，同时也是一个sysfs的顶层目录节点，
*		  在mount文件系统时会挂载到sys目录
*输入参数：无
*返回值：bus根节点
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
struct vein_bus_t * vein_bus_get_root(void)
{
	if(!__vein_bus_root)
		__vein_bus_root = vein_bus_alloc_directory("bus");
	return __vein_bus_root;
}

/********************************************************************
*                      功能函数
*功能描述： 从一个父节点中搜索一个名为name的子节点。
*输入参数：
*		parent: 父节点
*		name:名称
*返回值：name对应的位置
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
struct vein_bus_t * vein_bus_search(struct vein_bus_t * parent, const char * name)
{
	struct vein_bus_t * pos, * n;//pos位置,临时变量n
	if(!name)//名字不为空
	 {
		VEIN_LOG_TRACE("name is null!\n");
		return NULL;
	 }

	if(!parent)//参数不为空
	 {
		VEIN_LOG_TRACE("%s parent is null!\n",name);
		return NULL;
	 }

	if(parent->type != VEIN_BUS_TYPE_DIR)//类型不是目录
	 {
		VEIN_LOG_TRACE("parent type not VEIN_BUS_TYPE_DIR!\n");
		return NULL;
	 }

	list_for_each_entry_safe(pos, n, struct vein_bus_t,&(parent->children), entry)//循环查找
	 {
		if(strcmp(pos->name, name) == 0)
			return pos;
	 }

	return NULL;
}

/********************************************************************
*                      功能函数
*功能描述：在父节点下未搜索到名为name的子节点时,自动创建一个子节点,该节点为目录类型
*输入参数：
*		parent: 父节点 
*		name: 名称
*返回值：子节点
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
struct vein_bus_t * vein_bus_search_directory_with_create(struct vein_bus_t * parent, const char * name)
{
	struct vein_bus_t * bus;

	if(!parent)
	 {
		VEIN_LOG_TRACE("parent is NULL!\n");
		return NULL;
	 }
	if(parent->type != VEIN_BUS_TYPE_DIR)
	 {
		VEIN_LOG_TRACE("parent type not VEIN_BUS_TYPE_DIR!\n");
		return NULL;
	 }
	if(!name)
	 {
		VEIN_LOG_TRACE("name is NULL!\n");
		return NULL;
	 }

	bus = vein_bus_search(parent, name);
	if(bus == NULL)
	 {
		bus = vein_bus_alloc_directory(name);
		if(!bus)
		 {
			VEIN_LOG_TRACE("bus malloc fail!\n");
			return NULL;
		 }

		if(vein_bus_add(parent, bus) != VEIN_OK)
		 {
			VEIN_LOG_TRACE("bus add fail!\n");
			vein_bus_free(bus);
			return NULL;
		 }
	 }
	else if(bus->type != VEIN_BUS_TYPE_DIR)
	 {
		VEIN_LOG_TRACE("bus type not VEIN_BUS_TYPE_DIR!\n");
		return NULL;
	 }

	return bus;
}

/********************************************************************
*                      功能函数
*功能描述： 其为内部函数__vein_bus_alloc的二次封装，快速分配指定类型的目录节点
*输入参数：name:名称
*返回值：bus
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
struct vein_bus_t * vein_bus_alloc_directory(const char * name)
{
	return __vein_bus_alloc(name, VEIN_BUS_TYPE_DIR, NULL);
}

/********************************************************************
*                      功能函数
*功能描述： 其为内部函数__bus_alloc的二次封装，快速分配指定类型的文件节点
*输入参数：
*		name: 名称
*		priv: 私有数据块
*返回值：bus
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
struct vein_bus_t * vein_bus_alloc_regular(const char * name, void * priv)
{
	return __vein_bus_alloc(name, VEIN_BUS_TYPE_REG, priv);
}

/********************************************************************
*                      功能函数
*功能描述： bus动态分配的内存进行回收
*输入参数： bus
*返回值：FALSE: 失败、TRUE: 成功
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
int vein_bus_free(struct vein_bus_t * bus)
{
	if(!bus)
	 return VEIN_ERR;
	VEIN_FREE(bus->name);
	VEIN_FREE(bus);
	return VEIN_OK;
}

/********************************************************************
*                      功能函数
*功能描述： 将一个节点,可以是目录节点或文件节点,添加至父目录节点
*输入参数：
*		parent: 父节点
*		bus: obj
*返回值：成功返回真,否则返回假
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
int vein_bus_add(struct vein_bus_t * parent, struct vein_bus_t * bus)
{
	if(!parent)
	 {
		VEIN_LOG_TRACE("parent is NULL!\n");
		return VEIN_ERR;
	 }

	if(parent->type != VEIN_BUS_TYPE_DIR)
	 {
		VEIN_LOG_TRACE("parent type not VEIN_BUS_TYPE_DIR!\n");
		return VEIN_ERR;
	 }

	if(!bus)
	 {
		VEIN_LOG_TRACE("bus is NULL!\n");
		return VEIN_ERR;
	 }

	if(vein_bus_search(parent, bus->name))
	 {
		VEIN_LOG_TRACE("parent not bus name child!\n");
		return VEIN_ERR;
	 }

	spin_lock_irq();
	bus->parent = parent;
	list_add_tail(&bus->entry, &parent->children);
	spin_unlock_irq();
	return VEIN_OK;
}

/********************************************************************
*                      功能函数
*功能描述： 从一个父目录节点中删除一个子节点
*输入参数：
*		parent: 父节点
*		bus : obj
*返回值：成功返回真,否则返回假
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
int vein_bus_remove(struct vein_bus_t * parent, struct vein_bus_t * bus)
{
	struct vein_bus_t * pos, * n;
	if(!parent)
	 {
		VEIN_LOG_TRACE("parent is NULL!\n");
		return VEIN_ERR;
	 }

	if(parent->type != VEIN_BUS_TYPE_DIR)
	 {
		VEIN_LOG_TRACE("parent type not VEIN_BUS_TYPE_DIR!\n");
		return VEIN_ERR;
	 }

	if(!bus)
	 {
		VEIN_LOG_TRACE("bus is NULL!\n");
		return VEIN_ERR;
	 }

	list_for_each_entry_safe(pos, n, struct vein_bus_t, &(parent->children), entry)
	 {
		if(pos == bus)
		 {
			spin_lock_irq();
			pos->parent = pos;
			list_del(&(pos->entry));
			spin_unlock_irq();
			return VEIN_OK;
		 }
	 }

	return VEIN_ERR;
}

/********************************************************************
*                      功能函数
*功能描述：此接口实现的目的是为了快速添加子节点，其根据所传递的参数，
*			自动创建节点并添加到父节点上。
*输入参数：
*		parent: 父节点
*		name: 名称
*返回值：成功返回真,否则返回假
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
int vein_bus_add_directory(struct vein_bus_t * parent, const char * name)
{
	struct vein_bus_t * bus;

	if(!parent)
	 {
		VEIN_LOG_TRACE("parent is NULL!\n");
		return VEIN_ERR;
	 }

	if(parent->type != VEIN_BUS_TYPE_DIR)
	 {
		VEIN_LOG_TRACE("parent type not VEIN_BUS_TYPE_DIR!\n");
		return VEIN_ERR;
	 }

	if(!name)
	 {
		VEIN_LOG_TRACE("name is NULL!\n");
		return VEIN_ERR;
	 }

	if(vein_bus_search(parent, name))
	 {
		VEIN_LOG_TRACE("in parent not search name!\n");
		return VEIN_ERR;
	 }

	bus = vein_bus_alloc_directory(name);
	if(!bus)
	 {
		VEIN_LOG_TRACE("bus malloc fail!\n");
		return VEIN_ERR;
	 }

	if(vein_bus_add(parent, bus) != VEIN_OK)
	  vein_bus_free(bus);

	return VEIN_OK;
}

/********************************************************************
*                      功能函数
*功能描述：添加子文件节点
*输入参数：
*		parent: 父节点
*		name: 名称
*		priv: 私有数据块
*返回值：成功返回真,否则返回假
*其他说明：无
*修改日期          版本        修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
int vein_bus_add_regular(struct vein_bus_t * parent, const char * name, void * priv)
{
	struct vein_bus_t * bus;

	if(!parent)
	 {
		VEIN_LOG_TRACE("parent is NULL!\n");
		return VEIN_ERR;
	 }

	if(parent->type != VEIN_BUS_TYPE_DIR)
	 {
		VEIN_LOG_TRACE("parent type not VEIN_BUS_TYPE_DIR!\n");
		return VEIN_ERR;
	 }

	if(!name)
	 {
		VEIN_LOG_TRACE("name is NULL!\n");
		return VEIN_ERR;
	 }

	if(vein_bus_search(parent, name))
	 {
		VEIN_LOG_TRACE("in parent not search name!\n");
		return VEIN_ERR;
	 }

	bus = vein_bus_alloc_regular(name, priv);
	if(!bus)
	 {
		VEIN_LOG_TRACE("bus malloc fail!\n");
		return VEIN_ERR;
	 }

	if(vein_bus_add(parent, bus) != VEIN_OK)
		vein_bus_free(bus);

	return VEIN_OK;
}

/********************************************************************
*                      功能函数
*功能描述：此接口会将自身及其所有子节点以递归方式删除
*输入参数：节点
*返回值：成功返回真,否则返回假
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
int vein_bus_remove_self(struct vein_bus_t * bus)
{
	struct vein_bus_t * parent;
	struct vein_bus_t * pos, * n;
	uint8_t ret;

	if(!bus)
	 {
		VEIN_LOG_TRACE("bus is NULL!\n");
		return VEIN_ERR;
	 }

	if(bus->type == VEIN_BUS_TYPE_DIR)
	 {
		list_for_each_entry_safe(pos, n, struct vein_bus_t,&(bus->children), entry)
		 {
			vein_bus_remove_self(pos);
		 }
	 }

	parent = bus->parent;
	if(parent && (parent != bus))
	 {
		ret = vein_bus_remove(parent, bus);
		if(ret)
		  vein_bus_free(bus);
		return ret;
	 }

	vein_bus_free(bus);
	return VEIN_OK;
}

/********************************************************************
*                      功能函数
*功能描述：根据字符串动态分配hash值
*输入参数：字符串
*返回值：返回value
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.19     1.0        刘杨
**********************************************************************/
uint32_t shash(const char * s)
{
	uint32_t v = 5381;
	if(s)
	 {
		while(*s)
		  v = (v << 5) + v + (*s++);
	 }
	VEIN_LOG_TRACE("string: %s,value: %d\n",s,v);
	return v;
}
