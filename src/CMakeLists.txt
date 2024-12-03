set(CMAKE_CACHEFILE_DIR ${CMAKE_CURRENT_LIST_DIR}/build)

# raspberry_pi
add_executable (raspberry_pi_client
  ${SENSOR_EXTENSIONS}
  ${CMAKE_CURRENT_LIST_DIR}/raspberry_pi_client/main.cpp
)

target_link_libraries(raspberry_pi_client "${PAHO_MQTT_C}" dotenv)