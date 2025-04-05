cmake_minimum_required(VERSION 3.8)
project(bt_test)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_cascade_lifecycle REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sound_msgs REQUIRED)
find_package(audio_common_msgs REQUIRED)
find_package(bt_nodes REQUIRED)

set(dependencies
  rclcpp
  rclcpp_cascade_lifecycle
  rclcpp_action
  behaviortree_cpp_v3
  audio_common_msgs
  sound_msgs
  std_msgs
  geometry_msgs
  bt_nodes
)

add_executable(is_detected_test src/is_detected_test.cpp)
ament_target_dependencies(is_detected_test ${dependencies})
target_link_libraries(is_detected_test bt_nodes::is_sound_detected_bt_node)

install(TARGETS
  is_detected_test
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY bt_xml
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_dependencies(${dependencies})

ament_package()
