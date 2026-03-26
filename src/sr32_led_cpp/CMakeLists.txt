cmake_minimum_required(VERSION 3.8)
project(sr32_led_cpp)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

include_directories(
  include
)

add_library(led_controller_lib
  src/led_controller.cpp
)

ament_target_dependencies(led_controller_lib
  rclcpp
  std_msgs
)

target_include_directories(led_controller_lib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

add_executable(sr32_led_node
  src/sr32_led_node.cpp
)

add_executable(test_sil_standalone
  src/test_sil_standalone.cpp
)

target_link_libraries(test_sil_standalone
  led_controller_lib
)

ament_target_dependencies(test_sil_standalone
  rclcpp
  std_msgs
)

target_link_libraries(sr32_led_node
  led_controller_lib
)

ament_target_dependencies(sr32_led_node
  rclcpp
  std_msgs
)

install(
  DIRECTORY include/
  DESTINATION include
)

install(
  TARGETS
    led_controller_lib
    sr32_led_node
    test_sil_standalone
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

ament_export_include_directories(include)
ament_export_targets(export_${PROJECT_NAME})

ament_package()
