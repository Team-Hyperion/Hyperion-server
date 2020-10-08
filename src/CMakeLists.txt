# CMakeList.txt : CMake project for Hyperion-server, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.9)

log_msg("")
log_msg("[   Hyperion   ]")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR})

hyperion_show_properties()


set(HYPERION_DIR ${PROJECT_SOURCE_DIR}/src)
# ======================================== Header files .h
set(HYPERION_INCLUDE_FILES
        ${PROJECT_SOURCE_DIR}/include
        )
# ======================================== END Header files .h
# ======================================== Source files .cpp
set(HYPERION_SRC_FILES
		${HYPERION_DIR}/core/crash_handler.cpp
        ${HYPERION_DIR}/core/execution_timer.cpp
        ${HYPERION_DIR}/core/filesystem.cpp
        ${HYPERION_DIR}/core/logger.cpp
        ${HYPERION_DIR}/core/utility.cpp
        

        ${HYPERION_DIR}/net/connection.cpp
        ${HYPERION_DIR}/net/communication.cpp

        ${HYPERION_DIR}/net/service/connection_acceptor.cpp
        ${HYPERION_DIR}/net/service/connection_gc.cpp
        ${HYPERION_DIR}/net/service/connection_listener.cpp
        )
# ======================================== END Source files .cpp


# ======================================== Lib files .cpp
set(HYPERION_LIB_BUILD_FILES
        )

IF (CMAKE_SYSTEM_NAME MATCHES Windows)
    set(HYPERION_LIB_BUILD_FILES
            ${HYPERION_LIB_BUILD_FILES}
            ${PROJECT_SOURCE_DIR}/lib/StackWalker/Main/StackWalker/StackWalker.cpp
            )
ENDIF ()
# ======================================== END Lib files .cpp

IF (HYPERION_BUILD_TESTS OR NOT HYPERION_INTERPROCEDURAL_OPTIMIZATION)
    # library linked to for tests
    add_library(hyperionBase
            ${HYPERION_SRC_FILES}
            ${HYPERION_LIB_BUILD_FILES}
            )
    target_include_directories(hyperionBase PUBLIC ${HYPERION_INCLUDE_FILES})
    target_link_libraries(hyperionBase hyperionLib)
ENDIF ()


# Jactorio Executable
IF (HYPERION_INTERPROCEDURAL_OPTIMIZATION)
    # Must compile sources separately for IPO
    add_executable(hyperion
            ${HYPERION_DIR}/hyperion.cpp
            ${HYPERION_SRC_FILES}
            ${HYPERION_LIB_BUILD_FILES}
            )
    set_property(TARGET hyperion PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)

    target_link_libraries(hyperion hyperionLib)
    target_include_directories(hyperion PUBLIC ${HYPERION_INCLUDE_FILES})
ELSE ()
    add_executable(hyperion
            ${HYPERION_DIR}/hyperion.cpp
            )
    target_link_libraries(hyperion hyperionBase)
ENDIF ()
