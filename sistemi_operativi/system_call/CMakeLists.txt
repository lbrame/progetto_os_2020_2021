cmake_minimum_required(VERSION 3.17)
project(progetto_os_2020_2021 C)

add_library(
    os_core

    ${CMAKE_CURRENT_SOURCE_DIR}/src/defines.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/defines.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/err_exit.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/err_exit.h

    # Sorgenti non ancora implementati 
    #   ${CMAKE_CURRENT_SOURCE_DIR}/fifo.c
    #   ${CMAKE_CURRENT_SOURCE_DIR}/fifo.h
    #   ${CMAKE_CURRENT_SOURCE_DIR}/pipe.c
    #   ${CMAKE_CURRENT_SOURCE_DIR}/pipe.h
    #   ${CMAKE_CURRENT_SOURCE_DIR}/semaphore.c
    #   ${CMAKE_CURRENT_SOURCE_DIR}/semaphore.h
)

target_include_directories(
    os_core
    SYSTEM
    PUBLIC

    ${CMAKE_CURRENT_SOURCE_DIR}/src/
)

target_compile_features(
    os_core
    PUBLIC

    c_std_99
)

add_executable(
    senderManager

    ${CMAKE_CURRENT_SOURCE_DIR}/src/sender_manager.c
)

add_executable(
    receiverManager

    ${CMAKE_CURRENT_SOURCE_DIR}/src/receiver_manager.c
)

add_executable(
    hackler

    ${CMAKE_CURRENT_SOURCE_DIR}/src/hackler.c
)

add_executable(
    S1

    ${CMAKE_CURRENT_SOURCE_DIR}/src/S1.c
)

add_executable(
    S2

    ${CMAKE_CURRENT_SOURCE_DIR}/src/S2.c
)

add_executable(
    S3

    ${CMAKE_CURRENT_SOURCE_DIR}/src/S3.c
)


target_link_libraries(senderManager PUBLIC os_core)
target_link_libraries(receiverManager PUBLIC os_core)
target_link_libraries(hackler PUBLIC os_core)
target_link_libraries(S1 PUBLIC os_core)
target_link_libraries(S2 PUBLIC os_core)
target_link_libraries(S3 PUBLIC os_core)