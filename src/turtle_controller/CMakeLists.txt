cmake_minimum_required(VERSION 3.8)
project(turtle_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Dependências
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(turtlesim REQUIRED)

# Executável do nó turtle
add_executable(turtle src/turtle.cpp)

ament_target_dependencies(turtle
  rclcpp
  geometry_msgs
  std_msgs
  turtlesim
)

# Executável do nó send_msg
add_executable(send_msg src/send_msg.cpp)

ament_target_dependencies(send_msg
  rclcpp
  std_msgs
)

# Instala o executável
install(TARGETS
  turtle
  send_msg
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)

  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)

  ament_lint_auto_find_test_dependencies()
endif()

ament_package()