cmake_minimum_required(VERSION 3.10)

project(mqtt_samples LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# link_libraries(mosquitto)

# set(MOSQUITTO_CLIENT_EXTENSIONS_DIR ${CMAKE_CURRENT_LIST_DIR}/mqttclients/c/mosquitto_client_extensions)
# file(GLOB MOSQUITTO_CLIENT_EXTENSIONS ${MOSQUITTO_CLIENT_EXTENSIONS_DIR}/*.c)
# include_directories(${MOSQUITTO_CLIENT_EXTENSIONS_DIR})

# Find the Paho MQTT C library
find_library(PAHO_MQTT_C paho-mqtt3c)

# find_library(PAHO_MQTT_C NAMES paho-mqtt3c
#              HINTS "/path/to/paho/installation")

set(SENSOR_EXTENSIONS_DIR ${CMAKE_CURRENT_LIST_DIR}/sensors_file)
file(GLOB SENSOR_EXTENSIONS ${SENSOR_EXTENSIONS_DIR}/*.c ${SENSOR_EXTENSIONS_DIR}/*.cpp)
include_directories(${SENSOR_EXTENSIONS_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(${PRESET_PATH})