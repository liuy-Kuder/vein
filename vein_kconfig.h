#ifndef  __VEIN_KCONFIG_H_
#define  __VEIN_KCONFIG_H_

#if defined CONFIG_VEIN_NAME_MAX_SIZE
    #define DEVICE_NAME_SIZE			    CONFIG_VEIN_NAME_MAX_SIZE //设备名字长度，包含ID
#endif

#if defined CONFIG_VEIN_DEVICE_HASH_SIZE
    #define DEVICE_HASH_SIZE				CONFIG_VEIN_DEVICE_HASH_SIZE //设备类型
#endif

#if defined CONFIG_VEIN_DRIVER_HASH_SIZE
    #define DRIVER_HASH_SIZE				CONFIG_VEIN_DRIVER_HASH_SIZE //驱动类型
#endif

//is enable log
#if defined CONFIG_VEIN_USE_LOG
    #define VEIN_USE_LOG
    #define VEIN_LOG_LEVEL    CONFIG_VEIN_LOG_LEVEL
    #if defined CONFIG_VEIN_LOG_PRINTF
        #define VEIN_LOG_PRINTF   CONFIG_VEIN_LOG_PRINTF
    #endif
#endif

#if defined CONFIG_VEIN_USE_OS
    #define USE_OS
    #if defined CONFIG_VEIN_USE_FREERTOS
        #define USE_FREERTOS
    #elif defined CONFIG_VEIN_USE_THREADX
        #define USE_THREADX
    #else
      "error no select os!!!"
    #endif
#endif

#ifdef USE_OS
    #ifdef USE_FREERTOS
        #include "freertos/FreeRTOS.h"
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
