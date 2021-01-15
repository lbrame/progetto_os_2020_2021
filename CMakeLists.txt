cmake_minimum_required(VERSION 3.17)
project(progetto_os_2020_2021 C)

set(CMAKE_C_STANDARD 99)

include_directories(src/sistemi_operativi/system_call)

add_executable(senderManager
        src/sistemi_operativi/system_call/defines.c
        src/sistemi_operativi/system_call/defines.h
        src/sistemi_operativi/system_call/err_exit.c
        src/sistemi_operativi/system_call/err_exit.h
#        src/sistemi_operativi/system_call/fifo.c
#        src/sistemi_operativi/system_call/fifo.h
#        src/sistemi_operativi/system_call/pipe.c
#        src/sistemi_operativi/system_call/pipe.h
#        src/sistemi_operativi/system_call/semaphore.c
#        src/sistemi_operativi/system_call/semaphore.h
        src/sistemi_operativi/system_call/sender_manager.c
#        src/sistemi_operativi/system_call/shared_memory.c
#        src/sistemi_operativi/system_call/shared_memory.h
        )

add_executable(receiverManager
        src/sistemi_operativi/system_call/defines.c
        src/sistemi_operativi/system_call/defines.h
        src/sistemi_operativi/system_call/err_exit.c
        src/sistemi_operativi/system_call/err_exit.h
#        src/sistemi_operativi/system_call/fifo.c
#        src/sistemi_operativi/system_call/fifo.h
#        src/sistemi_operativi/system_call/pipe.c
#        src/sistemi_operativi/system_call/pipe.h
        src/sistemi_operativi/system_call/receiver_manager.c
#        src/sistemi_operativi/system_call/semaphore.c
#        src/sistemi_operativi/system_call/semaphore.h
#        src/sistemi_operativi/system_call/shared_memory.c
#        src/sistemi_operativi/system_call/shared_memory.h
        )

add_executable(hackler
        src/sistemi_operativi/system_call/defines.c
        src/sistemi_operativi/system_call/defines.h
        src/sistemi_operativi/system_call/err_exit.c
        src/sistemi_operativi/system_call/err_exit.h
        # src/sistemi_operativi/system_call/fifo.c
        # src/sistemi_operativi/system_call/fifo.h
        src/sistemi_operativi/system_call/hackler.c
        # src/sistemi_operativi/system_call/pipe.c
        # src/sistemi_operativi/system_call/pipe.h
        # src/sistemi_operativi/system_call/semaphore.c
        # src/sistemi_operativi/system_call/semaphore.h
        # src/sistemi_operativi/system_call/shared_memory.h
        )
