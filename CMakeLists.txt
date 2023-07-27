cmake_minimum_required(VERSION 3.5)
project(receiver)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

# find dependencies
find_package(rclcpp REQUIRED)
find_package(ament_cmake REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

include_directories(include)

add_executable(receiver src/receiver.cpp)
ament_target_dependencies(receiver rclcpp std_msgs sensor_msgs)

install(TARGETS
  receiver
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
