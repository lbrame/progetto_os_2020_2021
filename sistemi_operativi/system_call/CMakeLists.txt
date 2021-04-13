cmake_minimum_required(VERSION 3.17)
project(progetto_os_2020_2021 C)

add_library(
    os_core

    ${CMAKE_CURRENT_SOURCE_DIR}/defines.c
    ${CMAKE_CURRENT_SOURCE_DIR}/defines.h
    ${CMAKE_CURRENT_SOURCE_DIR}/err_exit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/err_exit.h

    # Sorgenti non ancora implementati 
    #   ${CMAKE_CURRENT_SOURCE_DIR}/fifo.c
    #   ${CMAKE_CURRENT_SOURCE_DIR}/fifo.h
       ${CMAKE_CURRENT_SOURCE_DIR}/pipe.c
       ${CMAKE_CURRENT_SOURCE_DIR}/pipe.h
    #   ${CMAKE_CURRENT_SOURCE_DIR}/semaphore.c
    #   ${CMAKE_CURRENT_SOURCE_DIR}/semaphore.h
)

target_include_directories(
    os_core
    SYSTEM
    PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}/
)

target_compile_features(
    os_core
    PUBLIC

    c_std_99
)

add_executable(
    senderManager

    ${CMAKE_CURRENT_SOURCE_DIR}/sender_manager.c
)

add_executable(
    receiverManager

    ${CMAKE_CURRENT_SOURCE_DIR}/receiver_manager.c
)

add_executable(
    hackler

    ${CMAKE_CURRENT_SOURCE_DIR}/hackler.c
)

add_executable(
    S1

    ${CMAKE_CURRENT_SOURCE_DIR}/S1.c
)

add_executable(
    S2

    ${CMAKE_CURRENT_SOURCE_DIR}/S2.c
)

add_executable(
    S3

    ${CMAKE_CURRENT_SOURCE_DIR}/S3.c
)

add_executable(
    R1

    ${CMAKE_CURRENT_SOURCE_DIR}/R1.c
)

add_executable(
    R2

    ${CMAKE_CURRENT_SOURCE_DIR}/R2.c
)

add_executable(
    R3

    ${CMAKE_CURRENT_SOURCE_DIR}/R3.c
)

add_dependencies(
        senderManager
        S1
        S2
        S3
)

add_dependencies(
        receiverManager
        R1
        R2
        R3
)

target_link_libraries(senderManager PUBLIC os_core)
target_link_libraries(receiverManager PUBLIC os_core)
target_link_libraries(hackler PUBLIC os_core)
target_link_libraries(S1 PUBLIC os_core)
target_link_libraries(S2 PUBLIC os_core)
target_link_libraries(S3 PUBLIC os_core)
target_link_libraries(R1 PUBLIC os_core)
target_link_libraries(R2 PUBLIC os_core)
target_link_libraries(R3 PUBLIC os_core)