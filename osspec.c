/***************************************************************************//**
   @file osspec.c
   Файл содержит некоторые внутренние функции, которые скрывают вызовы ОС:
   функции работы с объектами синхронизации, потоками и т.д.

   @author Borisov Alexey <borisov@lcard.ru>
   @date   28.05.2012
   ***************************************************************************/
#include "osspec.h"
#include <stdlib.h>


#ifdef OSSPEC_USE_MUTEX
/***************************************************************************
  Вспомогательная функции для работы с мьютексами....
  **************************************************************************/
//создание мьютекса
t_mutex osspec_mutex_create(void) {
    t_mutex hnd;
#ifdef _WIN32
    hnd = CreateMutex(NULL, FALSE, NULL);
    if (hnd==NULL)
        hnd = L_INVALID_MUTEX;
#else
    hnd = (t_mutex)malloc(sizeof(pthread_mutex_t));
    if (hnd!=NULL) {
        int err = pthread_mutex_init(hnd, NULL);
        if (err) {
            free(hnd);
            hnd = L_INVALID_MUTEX;
        }
    } else {
        hnd = L_INVALID_MUTEX;
    }
#endif
    return hnd;
}



//захват мьютекса
int32_t osspec_mutex_lock(t_mutex handle, uint32_t tout) {
    int32_t res = 0;
    if (handle == L_INVALID_MUTEX)
        res = OS_SPEC_ERR_MUTEX_INVALID_HANDLE;
#ifdef _WIN32
    if (!res) {
        res = WaitForSingleObject(handle, tout)==WAIT_OBJECT_0 ? 0 : OS_SPEC_ERR_MUTEX_LOCK_TOUT;
    }
#else
    if (!res && pthread_mutex_lock(handle))
         res = OS_SPEC_ERR_MUTEX_LOCK_TOUT;
#endif
    return res;
}

//освобождение мьютекса
int32_t osspec_mutex_release(t_mutex handle) {
    int32_t res = 0;
    if (handle == L_INVALID_MUTEX)
        res = OS_SPEC_ERR_MUTEX_INVALID_HANDLE;
#ifdef _WIN32
    if (!res)
        res = ReleaseMutex(handle) ? 0 : OS_SPEC_ERR_MUTEX_RELEASE;
#else
    if (!res && pthread_mutex_unlock(handle))
        res = OS_SPEC_ERR_MUTEX_RELEASE;
#endif
    return res;
}

int32_t  osspec_mutex_close(t_mutex handle) {
    int32_t res = (handle != L_INVALID_MUTEX) ?
        0 : OS_SPEC_ERR_MUTEX_INVALID_HANDLE;
#ifdef _WIN32
    if (!res)
        CloseHandle(handle);
#else
    if (!res) {
        pthread_mutex_destroy(handle);
        free(handle);
    }
#endif
    return res;
}

#endif

