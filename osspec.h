/***************************************************************************//**
   @file osspec.h
   Файл содержит объявления функций, которые скрывают вызовы ОС:
   функции работы с объектами синхронизации, потоками и т.д.

   @author Borisov Alexey <borisov@lcard.ru>
   @date   28.05.2012
   ***************************************************************************/
#ifndef OSSPEC_H_
#define OSSPEC_H_

#include "osspec_cfg.h"

#define L_INVALID_MUTEX NULL

#ifdef _WIN32
    #include <Windows.h>
    typedef HANDLE t_mutex;
#else
    #include <pthread.h>
    typedef pthread_mutex_t* t_mutex;
#endif


t_mutex osspec_mutex_create(void);
int32_t osspec_mutex_lock(t_mutex handle, uint32_t tout);
int32_t osspec_mutex_release(t_mutex handle);
int32_t osspec_mutex_close(t_mutex handle);


#endif

