# CMakeList.txt : CMake project for hyperion TESTS, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

log_msg("")
log_msg("[Hyperion test ]")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_BINARY_DIR})

hyperion_show_properties()



####################
# Google Test
# Download and unpack googletest at configure time
configure_file(${PROJECT_SOURCE_DIR}/test/CMakeLists.txt.in googletest-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )

if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )

if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if(CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()



# Add source to this project's executable.
add_compile_definitions(HYPERION_BUILD_TEST)  # Disables warning when using test only functions

set(HYPERION_TEST_DIR ${PROJECT_SOURCE_DIR}/test)
# ======================================== Test files .cpp
set(HYPERION_TEST_FILES
	${HYPERION_TEST_DIR}/core/convertTests.cpp
	${HYPERION_TEST_DIR}/core/file_systemTests.cpp
	${HYPERION_TEST_DIR}/core/guarded_dataTests.cpp
	${HYPERION_TEST_DIR}/core/resource_guardTests.cpp
	${HYPERION_TEST_DIR}/core/utilityTests.cpp

	${HYPERION_TEST_DIR}/media/saveTests.cpp

	${HYPERION_TEST_DIR}/net/communicationTests.cpp
)
# ======================================== END Test files .cpp

# Test executable
add_executable(hyperionTest 
	${HYPERION_TEST_DIR}/hyperionTests.cpp
	${HYPERION_TEST_FILES}
)
target_link_libraries(hyperionTest 
	gtest_main hyperionBase
)
target_include_directories(hyperionTest
	PUBLIC
	${HYPERION_TEST_DIR}
)
