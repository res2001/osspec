/***************************************************************************//**
   @file osspec.c
   Файл содержит некоторые внутренние функции, которые скрывают вызовы ОС:
   функции работы с объектами синхронизации, потоками и т.д.

   @author Borisov Alexey <borisov@lcard.ru>
   @date   28.05.2012
   ***************************************************************************/
#include "osspec.h"
#include <stdlib.h>


#ifndef _WIN32
#include <sys/time.h>
#include <errno.h>

static void f_get_abs_time(uint32_t timeout, struct timespec *timeToWait) {
    clock_gettime(CLOCK_REALTIME, timeToWait);

    timeToWait->tv_sec += timeout/1000;
    timeToWait->tv_nsec += 1e6*((timeout+1)%1000);
    while (timeToWait->tv_nsec >= 1e9) {
        timeToWait->tv_sec ++;
        timeToWait->tv_nsec -=1e9;
    }
}
#endif

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
        hnd = OSSPEC_INVALID_MUTEX;
#else
    hnd = (t_mutex)malloc(sizeof(pthread_mutex_t));
    if (hnd!=NULL) {
        int err = pthread_mutex_init(hnd, NULL);
        if (err) {
            free(hnd);
            hnd = OSSPEC_INVALID_MUTEX;
        }
    } else {
        hnd = OSSPEC_INVALID_MUTEX;
    }
#endif
    return hnd;
}



//захват мьютекса
int32_t osspec_mutex_lock(t_mutex handle, uint32_t timeout) {
    int32_t err = 0;
    if (handle == OSSPEC_INVALID_MUTEX)
        err = OSSPEC_ERR_MUTEX_INVALID_HANDLE;
#ifdef _WIN32
    if (!res) {
        res = WaitForSingleObject(handle, timeout)==WAIT_OBJECT_0 ? 0 : OSSPEC_ERR_MUTEX_LOCK_TOUT;
    }
#else
    if (!err) {
        int wt_res;
        if (timeout == OSSPEC_TIMEOUT_INFINITY) {
            wt_res = pthread_mutex_lock(handle);
        } else {
            struct timespec timeToWait;
            f_get_abs_time(timeout, &timeToWait);
            wt_res = pthread_mutex_timedlock(handle, &timeToWait);
        }
        if (wt_res == ETIMEDOUT) {
            err = OSSPEC_ERR_MUTEX_LOCK_TOUT;
        } else if (wt_res != 0) {
            err = OSSPEC_ERR_MUTEX_INVALID_HANDLE;
        }
    }
#endif
    return err;
}

//освобождение мьютекса
int32_t osspec_mutex_release(t_mutex handle) {
    int32_t res = 0;
    if (handle == OSSPEC_INVALID_MUTEX)
        res = OSSPEC_ERR_MUTEX_INVALID_HANDLE;
#ifdef _WIN32
    if (!res)
        res = ReleaseMutex(handle) ? 0 : OSSPEC_ERR_MUTEX_RELEASE;
#else
    if (!res && pthread_mutex_unlock(handle))
        res = OSSPEC_ERR_MUTEX_RELEASE;
#endif
    return res;
}

int32_t  osspec_mutex_destroy(t_mutex handle) {
    int32_t res = (handle != OSSPEC_INVALID_MUTEX) ?
        0 : OSSPEC_ERR_MUTEX_INVALID_HANDLE;
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





#ifdef OSSPEC_USE_EVENTS
    #ifdef _WIN32

    #else
        struct st_osspec_event {
            pthread_cond_t cond;
            pthread_mutex_t mutex;
            int flags;
            int val;
        };
    #endif


    t_event osspec_event_create(int32_t flags) {
        t_event evt = malloc(sizeof(struct st_osspec_event));
        if (evt!=NULL) {
            if (pthread_mutex_init(&evt->mutex, NULL) != 0) {
                free(evt);
                evt = NULL;
            } else {
                if (pthread_cond_init(&evt->cond, NULL)!= 0) {
                    pthread_mutex_destroy(&evt->mutex);
                    free(evt);
                    evt = NULL;
                } else {
                    evt->flags = flags;
                    evt->val = 0;
                }
            }
        }
        return evt;
    }

    int32_t osspec_event_destroy(t_event event) {
        int32_t res = (event != OSSPEC_INVALID_EVENT) ?
            0 : OSSPEC_ERR_EVENT_INVALID_HANDLE;
    #ifdef _WIN32
        if (!res)
            CloseHandle(event);
    #else
        if (!res) {
            pthread_mutex_destroy(&event->mutex);
            pthread_cond_destroy(&event->cond);
            free(event);
        }
    #endif
        return res;
    }

    int32_t osspec_event_set(t_event event)  {
        int32_t res = (event != OSSPEC_INVALID_EVENT) ?
                     0 : OSSPEC_ERR_EVENT_INVALID_HANDLE;
        if (res == 0) {
            pthread_mutex_lock(&event->mutex);
            if (!event->val) {
                event->val = 1;
                pthread_cond_signal(&event->cond);
            }
            pthread_mutex_unlock(&event->mutex);
        }
        return res;
    }

    int32_t osspec_event_clear(t_event event) {
        int32_t res = (event != OSSPEC_INVALID_EVENT) ?
                     0 : OSSPEC_ERR_EVENT_INVALID_HANDLE;
        if (res == 0) {
            pthread_mutex_lock(&event->mutex);
            event->val = 0;
            pthread_mutex_unlock(&event->mutex);
        }
        return res;
    }

    int32_t osspec_event_wait(t_event event, uint32_t timeout) {
        int32_t err = (event != OSSPEC_INVALID_EVENT) ?
            0 : OSSPEC_ERR_EVENT_INVALID_HANDLE;
        if (err == 0) {
            struct timespec timeToWait;
            int out = 0;
            if (timeout != OSSPEC_TIMEOUT_INFINITY)
                f_get_abs_time(timeout, &timeToWait);

            while (!out && !err) {
                pthread_mutex_lock(&event->mutex);
                if (!event->val) {
                    int wait_res = timeout == OSSPEC_TIMEOUT_INFINITY ?
                                pthread_cond_wait(&event->cond,&event->mutex) :
                                pthread_cond_timedwait(&event->cond,&event->mutex, &timeToWait);
                    if (event->val) {
                        out = 1;
                    } else if (wait_res == ETIMEDOUT) {
                        err = OSSPEC_ERR_EVENT_WAIT_TOUT;
                    } else if (wait_res == EINVAL) {
                        err = OSSPEC_ERR_EVENT_INVALID_HANDLE;
                    }
                } else {
                    out = 1;
                }
                pthread_mutex_unlock(&event->mutex);
            }
        }
        return err;
    }

#endif


#ifdef OSSPEC_USE_THREADS
    t_thread osspec_thread_create(t_osspec_thread_func func, void *arg, uint32_t flags) {
        t_thread thread;
        if (pthread_create(&thread, NULL, func, arg) != 0)  {
            thread = OSSPEC_INVALID_THREAD;
        }
        return thread;
    }

    int32_t osspec_thread_wait(t_thread thread, uint32_t timeout) {
        int32_t err = (thread != OSSPEC_INVALID_THREAD) ?
            0 : OSSPEC_ERR_THREAD_INVALID_HANDLE;
        if (!err) {

            int wt_res;

            if (timeout != OSSPEC_TIMEOUT_INFINITY) {
                struct timespec timeToWait;
                f_get_abs_time(timeout, &timeToWait);
                wt_res = pthread_timedjoin_np(thread, NULL, &timeToWait);
            } else {
                wt_res = pthread_join(thread, NULL);
            }
            if (wt_res ==  ETIMEDOUT) {
                err = OSSPEC_ERR_THREAD_WAIT_TOUT;
            } else if (wt_res != 0) {
                err = OSSPEC_ERR_THREAD_INVALID_HANDLE;
            }
        }
        return err;
    }
#endif






