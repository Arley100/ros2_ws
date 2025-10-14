cmake_minimum_required(VERSION 3.8)
project(cpp_pubsub)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(cpp_publisher src/publisher.cpp)
ament_target_dependencies(cpp_publisher rclcpp std_msgs)

add_executable(cpp_subscriber src/subscriber.cpp)
ament_target_dependencies(cpp_subscriber rclcpp std_msgs)

install(TARGETS
  cpp_publisher
  cpp_subscriber
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
