cmake_minimum_required(VERSION 3.10)

project(mqtt_samples LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the Paho MQTT C library
find_library(PAHO_MQTT_C paho-mqtt3cs)

# Add libs directory to library search path
include_directories(${CMAKE_SOURCE_DIR}/libs)
link_directories(${CMAKE_SOURCE_DIR}/libs)
set(PROJECT_LIB_DIR ${CMAKE_SOURCE_DIR}/libs)

# Add dotenv library to project
add_library(dotenv SHARED IMPORTED)
set_target_properties(dotenv PROPERTIES IMPORTED_LOCATION ${PROJECT_LIB_DIR}/libdotenv.so)

# Add sensors files
set(SENSOR_EXTENSIONS_DIR ${CMAKE_CURRENT_LIST_DIR}/sensors_file)
file(GLOB SENSOR_EXTENSIONS ${SENSOR_EXTENSIONS_DIR}/*.c ${SENSOR_EXTENSIONS_DIR}/*.cpp)
include_directories(${SENSOR_EXTENSIONS_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(${PRESET_PATH})