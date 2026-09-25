cmake_minimum_required(VERSION 3.8)
project(role_election)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/Detection.msg"
  "msg/Role.msg"
)

ament_export_dependencies(rosidl_default_runtime)

add_executable(detector src/detector.cpp)

ament_target_dependencies(detector
  rclcpp
)

rosidl_get_typesupport_target(cpp_typesupport_target
  ${PROJECT_NAME}
  "rosidl_typesupport_cpp"
)

target_link_libraries(detector
  "${cpp_typesupport_target}"
)

add_executable(decisor src/decisor.cpp)

ament_target_dependencies(decisor
  rclcpp
)

target_link_libraries(decisor
  "${cpp_typesupport_target}"
)

add_executable(monitor src/monitor.cpp)

ament_target_dependencies(monitor
  rclcpp
)

target_link_libraries(monitor
  "${cpp_typesupport_target}"
)

install(TARGETS
  detector
  decisor
  monitor
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()