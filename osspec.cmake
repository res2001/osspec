# Файл для включения в проект на CMAKE.
# Перед включением необходимо определить переменные
#    OSSPEC_DIR   - путь к директории, где находится osspec.cmake
#    OSSPEC_USE_EVENTS - использовать события
#    OSSPEC_USE_THREADS - использовать потоки
#    OSSPEC_USE_MUTEX - использовать мьютексы
# После включения будут установлены следующие перменные:
#   OSSPEC_HEADERS      - используемые заголовочные файлы
#   OSSPEC_SOURCES      - используемые файлы исходных кодов
#   OSSPEC_DEFINITIONS  - используемые определения компилятора
cmake_policy(PUSH)

cmake_minimum_required(VERSION 2.6)


if(OSSPEC_USE_EVENTS OR OSSPEC_USE_THREADS OR OSSPEC_USE_MUTEX)
    find_package(Threads)
    message("thread libs: ${CMAKE_THREAD_LIBS_INIT}")
#    set(OSSPEC_LIBS ${CMAKE_THREAD_LIBS_INIT})
    unset(OSSPEC_LIBS)
    unset(OSSPEC_DEFINITIONS)
    set(OSSPEC_HEADERS ${OSSPEC_DIR}/osspec.h)
    set(OSSPEC_SOURCES ${OSSPEC_DIR}/osspec.c)
    if(CMAKE_USE_PTHREADS_INIT)
        include(CheckLibraryExists)
        if(OSSPEC_USE_MUTEX)
            set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} OSSPEC_USE_MUTEX)
            check_library_exists(pthread pthread_mutex_timedlock "" HAVE_PTHREAD_MUTEX_TIMEDLOCK)
            if(HAVE_PTHREAD_MUTEX_TIMEDLOCK)
                set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} HAVE_PTHREAD_MUTEX_TIMEDLOCK)
                #заглушки для _timedlock функций отстутсвуют в libc, поэтому в любом случае включаем pthread
                set(OSSPEC_LIBS ${OSSPEC_LIBS} ${CMAKE_THREAD_LIBS_INIT})
            endif(HAVE_PTHREAD_MUTEX_TIMEDLOCK)
        endif(OSSPEC_USE_MUTEX)

        if(OSSPEC_USE_THREADS)
            set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} OSSPEC_USE_THREADS)
            if (NOT OSSPEC_LIBS)
                set(OSSPEC_LIBS ${CMAKE_THREAD_LIBS_INIT})
            endif (NOT OSSPEC_LIBS)
            check_library_exists(pthread pthread_timedjoin_np "" HAVE_PTHREAD_TIMEDJOIN_NP)
            if(HAVE_PTHREAD_TIMEDJOIN_NP)
                set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} HAVE_PTHREAD_TIMEDJOIN_NP)
            endif(HAVE_PTHREAD_TIMEDJOIN_NP)
        endif(OSSPEC_USE_THREADS)
    else(CMAKE_USE_PTHREADS_INIT)
        if(OSSPEC_USE_MUTEX)
            set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} OSSPEC_USE_MUTEX)
        endif(OSSPEC_USE_MUTEX)
        if(OSSPEC_USE_THREADS)
            set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} OSSPEC_USE_THREADS)
        endif(OSSPEC_USE_THREADS)
    endif(CMAKE_USE_PTHREADS_INIT)

    if(OSSPEC_USE_EVENTS)
        set(OSSPEC_DEFINITIONS ${OSSPEC_DEFINITIONS} OSSPEC_USE_EVENTS)
    endif(OSSPEC_USE_EVENTS)
endif(OSSPEC_USE_EVENTS OR OSSPEC_USE_THREADS OR OSSPEC_USE_MUTEX)

message("osspec libs: ${OSSPEC_LIBS}")

cmake_policy(POP)
