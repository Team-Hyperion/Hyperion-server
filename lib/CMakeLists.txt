# Includes dependencies
cmake_minimum_required(VERSION 3.8)

log_msg("")
log_msg("[ Hyperion lib ]")

function(validate_dependency DEPENDENCY_PATH)
	file(GLOB RESULT ${CMAKE_SOURCE_DIR}/lib/${DEPENDENCY_PATH}/*)
	list(LENGTH RESULT RES_LEN)

	if((NOT EXISTS ${CMAKE_SOURCE_DIR}/lib/${DEPENDENCY_PATH}) OR (RES_LEN EQUAL 0))
		log_msg("Installing missing dependency ${DEPENDENCY_PATH}")

		execute_process(COMMAND git submodule update --init --recursive -- lib/${DEPENDENCY_PATH}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
	else()
		log_msg("Found dependency: ${DEPENDENCY_PATH}")
	endif()
endfunction()





log_msg("Resolving dependencies...")
validate_dependency(StackWalker)
validate_dependency(backward-cpp)


# Ones which need to be built



set(HYPERION_LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
add_library(hyperionLib
		${HYPERION_LIB_DIR}/null.cpp  # CMake requires at least 1 cpp file
)


# Platform link libraries
set(
	LIBS_COMMON
)

if (WIN32)
	log_msg("Link Target: Windows")
	set(
		HYPERION_LINK_LIBS

		${LIBS_COMMON}
	)
elseif (APPLE)
	log_msg("Link Target: Apple")
    set(
		HYPERION_LINK_LIBS

		${LIBS_COMMON}
	)
elseif (UNIX)
	log_msg("Link Target: UNIX (Not Apple)")
    set(
		HYPERION_LINK_LIBS

		${LIBS_COMMON}
	)

	# For backward-cpp
	find_library(LIBDW_DEV dw)
	if(LIBDW_DEV)
		set(
			HYPERION_LINK_LIBS
			${HYPERION_LINK_LIBS}
			dw
		)
		target_compile_definitions(hyperionLib PUBLIC BACKWARD_HAS_DW=1)
	endif()
endif (WIN32)


# ======================================== Lib HEADER files .h
set(HYPERION_LIB_HEADER_FILES
	${PROJECT_SOURCE_DIR}/lib/StackWalker/Main
	${PROJECT_SOURCE_DIR}/lib/backward-cpp
)
# ======================================== END HEADER files .h

target_link_libraries(hyperionLib ${HYPERION_LINK_LIBS})
target_include_directories(hyperionLib PUBLIC ${HYPERION_LIB_HEADER_FILES})

