/***************************************************************************//**
   @file osspec.h
   Файл содержит объявления функций, которые скрывают вызовы ОС:
   функции работы с объектами синхронизации, потоками и т.д.

   @author Borisov Alexey <borisov@lcard.ru>
   @date   28.05.2012
   ***************************************************************************/
#ifndef OSSPEC_H_
#define OSSPEC_H_

#ifdef __cplusplus
extern "C" {
#endif

#if !defined _WIN32 && !defined _GNU_SOURCE
    #define _GNU_SOURCE
#endif

#include "osspec_cfg.h"

#ifdef _WIN32
    #include <Windows.h>
    #define OSSPEC_TIMEOUT_INFINITY  INFINITE
#else
    #include <pthread.h>
    #define OSSPEC_TIMEOUT_INFINITY  ((uint32_t)-1)
#endif


#ifdef OSSPEC_USE_MUTEX
    #ifdef _WIN32
        typedef HANDLE t_mutex;
    #else
        typedef pthread_mutex_t* t_mutex;
    #endif

    #define OSSPEC_INVALID_MUTEX NULL

    t_mutex osspec_mutex_create(void);
    int32_t osspec_mutex_lock(t_mutex handle, uint32_t tout);
    int32_t osspec_mutex_release(t_mutex handle);
    int32_t osspec_mutex_destroy(t_mutex handle);
#endif

#ifdef OSSPEC_USE_EVENTS
    #ifdef _WIN32
        typedef HANDLE t_event;
    #else
        struct st_osspec_event;
        typedef struct st_osspec_event *t_event;
    #endif

    #define OSSPEC_INVALID_EVENT NULL

    t_event osspec_event_create(int32_t flags);
    int32_t osspec_event_destroy(t_event event);
    int32_t osspec_event_set(t_event event);
    int32_t osspec_event_clear(t_event event);
    int32_t osspec_event_wait(t_event event, uint32_t timeout);
#endif


#ifdef OSSPEC_USE_THREADS
    #ifdef _WIN32
        typedef HANDLE t_thread;

        #define OSSPEC_INVALID_THREAD NULL

        #define OSSPEC_THREAD_FUNC_RET  DWORD
        #define OSSPEC_THREAD_FUNC_CALL WINAPI
    #else
        typedef pthread_t t_thread;

        #define OSSPEC_INVALID_THREAD ((pthread_t)-1)

        #define OSSPEC_THREAD_FUNC_RET void*
        #define OSSPEC_THREAD_FUNC_CALL
    #endif

    typedef OSSPEC_THREAD_FUNC_RET (OSSPEC_THREAD_FUNC_CALL *t_osspec_thread_func)(void *arg);

    t_thread osspec_thread_create(t_osspec_thread_func func, void *arg, uint32_t flags);
    int32_t  osspec_thread_wait(t_thread thread, uint32_t timeout);
#endif


#ifdef __cplusplus
}
#endif

#endif

