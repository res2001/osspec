# Файл для включения в проект на CMAKE.
# Перед включением необходимо определить переменные
#    OSSPEC_DIR   - путь к директории, где находится osspec.cmake
#    NO_OSSPEC_USE_EVENTS - не использовать события
#    NO_OSSPEC_USE_THREADS - не использовать потоки
#    NO_OSSPEC_USE_MUTEX - не использовать мьютексы
# После включения будут установлены следующие перменные:
#   OSSPEC_HEADERS      - используемые заголовочные файлы
#   OSSPEC_SOURCES      - используемые файлы исходных кодов
#   OSSPEC_DEFINITIONS  - используемые определения компилятора
cmake_policy(PUSH)

cmake_minimum_required(VERSION 2.6)

unset(OSSPEC_HEADERS CACHE)
unset(OSSPEC_SOURCES CACHE)
#unset(OSSPEC_LIBS CACHE)
unset(OSSPEC_DEFINITIONS CACHE)

if(NOT NO_OSSPEC_USE_EVENTS OR NOT NO_OSSPEC_USE_THREADS OR NOT NO_OSSPEC_USE_MUTEX)
    find_package(Threads REQUIRED)
#    set(OSSPEC_LIBS ${CMAKE_THREAD_LIBS_INIT})
    set(OSSPEC_HEADERS ${OSSPEC_DIR}/osspec.h)
    set(OSSPEC_SOURCES ${OSSPEC_DIR}/osspec.c)
    if(CMAKE_USE_PTHREADS_INIT)
        include(CheckLibraryExists)
        if(NO_OSSPEC_USE_MUTEX)
            set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} NO_OSSPEC_USE_MUTEX)
        else(NO_OSSPEC_USE_MUTEX)
            check_library_exists(pthread pthread_mutex_timedlock "" HAVE_PTHREAD_MUTEX_TIMEDLOCK)
            if(HAVE_PTHREAD_MUTEX_TIMEDLOCK)
                set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} HAVE_PTHREAD_MUTEX_TIMEDLOCK)
            endif(HAVE_PTHREAD_MUTEX_TIMEDLOCK)
        endif(NO_OSSPEC_USE_MUTEX)

        if(NO_OSSPEC_USE_THREADS)
            set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} NO_OSSPEC_USE_THREADS)
        else(NO_OSSPEC_USE_THREADS)
            check_library_exists(pthread pthread_timedjoin_np "" HAVE_PTHREAD_TIMEDJOIN_NP)
            if(HAVE_PTHREAD_TIMEDJOIN_NP)
                set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} HAVE_PTHREAD_TIMEDJOIN_NP)
            endif(HAVE_PTHREAD_TIMEDJOIN_NP)
        endif(NO_OSSPEC_USE_THREADS)
    else(CMAKE_USE_PTHREADS_INIT)
        if(NO_OSSPEC_USE_MUTEX)
            set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} NO_OSSPEC_USE_MUTEX)
        endif(NO_OSSPEC_USE_MUTEX)
        if(NO_OSSPEC_USE_THREADS)
            set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} NO_OSSPEC_USE_THREADS)
        endif(NO_OSSPEC_USE_THREADS)
    endif(CMAKE_USE_PTHREADS_INIT)

    if(NO_OSSPEC_USE_EVENTS)
        set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} NO_OSSPEC_USE_EVENTS)
    endif(NO_OSSPEC_USE_EVENTS)
endif(NOT NO_OSSPEC_USE_EVENTS OR NOT NO_OSSPEC_USE_THREADS OR NOT NO_OSSPEC_USE_MUTEX)

cmake_policy(POP)
