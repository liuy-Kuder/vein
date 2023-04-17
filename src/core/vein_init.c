#include "vein.h"
/********************************************************************
*                      功能函数
*功能描述：vein组件初始化
*		  1、总线资源初始化
*		  2、驱动资源初始化
*输入参数：无
*返回值：无
*其他说明：无
*修改日期       版本      修改人        修改内容
*---------------------------------------------------------------------
*2022.9.12     1.0        刘杨
**********************************************************************/
int vein_init(void)
{
    vein_driver_pure_init();//初始化总线列表
    vein_device_pure_init();//初始化驱动列表
    VEIN_LOG_INFO("Vein v%d.%d.%d %s\n" VEIN_VERSION_INFO,VEIN_VERSION_MAJOR,VEIN_VERSION_MINOR,VEIN_VERSION_PATCH);
    return 0;
}

