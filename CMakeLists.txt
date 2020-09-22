cmake_minimum_required(VERSION 3.9)
include(CheckIPOSupported)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("Hyperion-server")

option(HYPERION_BUILD_TESTS "HYPERION_BUILD_TESTS")



function(log_msg message)
	message(" - ${message}")
endfunction()

# Shows properties for a subdirectory
function(hyperion_show_properties)
	log_msg("Binary directory: ${CMAKE_CURRENT_BINARY_DIR}")
endfunction()


log_msg("Target: ${CMAKE_SYSTEM_NAME}")


IF(CMAKE_BUILD_TYPE MATCHES Debug)
	log_msg("Type: Debug")
    add_compile_definitions(HYPERION_DEBUG_BUILD)  # Break on openGL error

ELSEIF(CMAKE_BUILD_TYPE MATCHES Release OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
	log_msg("Type: <<< Release >>>")
	check_ipo_supported(RESULT result)

	if(result)
		log_msg("IPO: Available")
		set(HYPERION_INTERPROCEDURAL_OPTIMIZATION TRUE)
	else()
		log_msg("IPO: ~Unavailable")
	endif()
ELSE()
  message(FATAL_ERROR "Unknown build type, specify with -D CMAKE_BUILD_TYPE=")
ENDIF()




add_subdirectory("lib")

# ======================================== Defines
add_compile_definitions(HYPERION_BUILD_TARGET_PLATFORM="${CMAKE_SYSTEM_NAME}")

# ======================================== Options
add_compile_definitions(HYPERION_VERSION="0.1.0")
add_compile_definitions(HYPERION_LOG_LEVEL=0)
# 0 - Debug, 1 - Info, 2 - Warning, 3 - Error, 4 - Critical, 5 - None


add_subdirectory("src")
if (HYPERION_BUILD_TESTS)
	add_subdirectory("test")
endif ()