#ifndef  __VEIN_CONF_H_
#define  __VEIN_CONF_H_

#ifndef CONFIG_VEIN_CONF_SKIP

#define DEVICE_NAME_SIZE				(32) //设备名字长度，包含ID
#define DEVICE_HASH_SIZE				(64) //设备类型
#define DRIVER_HASH_SIZE				(64) //驱动类型
//is enable log
#define VEIN_USE_LOG
#define VEIN_LOG_LEVEL    VEIN_LOG_LEVEL_WARN
#define VEIN_LOG_PRINTF   1

#define NOUSE_OS
//#define USE_OS
//#define USE_FREERTOS
//#define USE_THREADX

#ifdef USE_OS
    #ifdef USE_FREERTOS
        #include "FreeRTOS.h"
        #include "semphr.h"
        #define spin_lock_irq()     portDISABLE_INTERRUPTS()
        #define spin_unlock_irq()   portENABLE_INTERRUPTS()

        #define VEIN_MALLOC(X)    pvPortMalloc(X)
        #define VEIN_FREE(X)      vPortFree(X)
    #elif #define USE_THREADX
        #if 0
        sample:
        uint8_t VeinPoolBuf[1024*30];//30k
        TX_BYTE_POOL Vein_pool;

            /* 创建内存池，类似malloc和free */    
            tx_byte_pool_create(&Vein_pool,
                    "Vein_pool",
                    (VOID *)VeinPoolBuf,     /* 内存池地址，需要保证4字节对齐 */
                    sizeof(VeinPoolBuf));    /* 内存池大小 */
        #endif
        #include "tx_api.h"
        #define spin_lock_irq()     TX_INTERRUPT_SAVE_AREA; TX_DISABLE
        #define spin_unlock_irq()   TX_RESTORE
        extern TX_BYTE_POOL Vein_pool;
        static void * tx_malloc(size_t len)//二次封装
        {
            void  *Ptr;
            uint8_t r = tx_byte_allocate(&Vein_pool,&Ptr,len,TX_NO_WAIT);
            if(r != TX_SUCCESS)
            {
                Ptr = NULL;
            }
            return Ptr;
        }
        #define VEIN_MALLOC(X)    tx_malloc(X)
        #define VEIN_FREE(X)      tx_byte_release(X)
    #endif
#else //NO USE_OS
    #define spin_lock_irq()                   //  __disable_irq()
    #define spin_unlock_irq()                 //  __enable_irq()
    #define VEIN_MALLOC(len)     malloc(len)
    #define VEIN_FREE(addr)      free(addr)
#endif

#endif
#endif
