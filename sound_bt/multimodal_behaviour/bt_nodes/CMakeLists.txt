cmake_minimum_required(VERSION 3.8)
project(bt_nodes)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_cascade_lifecycle REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)
find_package(audio_common_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sound_msgs REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(yolo_msgs REQUIRED)


set(dependencies
  rclcpp
  rclcpp_cascade_lifecycle
  rclcpp_action
  behaviortree_cpp_v3
  audio_common_msgs
  std_msgs
  std_srvs
  sound_msgs
  tf2_ros
  tf2_geometry_msgs
  tf2
  visualization_msgs
  yolo_msgs
)

include_directories(include)

add_library(speak_bt_node SHARED src/Speak.cpp)
list(APPEND plugin_libs speak_bt_node)
add_library(is_sound_detected_bt_node SHARED src/IsSoundDetected.cpp)
list(APPEND plugin_libs is_sound_detected_bt_node)
add_library(rotate_bt_node SHARED src/Rotate.cpp)
list(APPEND plugin_libs rotate_bt_node)
add_library(is_human_detected_bt_node SHARED src/IsHumanDetected.cpp)
list(APPEND plugin_libs is_human_detected_bt_node)
add_library(attend_bt_node SHARED src/Attend.cpp)
list(APPEND plugin_libs attend_bt_node)
add_library(define_interaction_bt_node SHARED src/DefineInteraction.cpp)
list(APPEND plugin_libs define_interaction_bt_node)
add_library(is_audio_located_bt_node SHARED src/IsAudioLocated.cpp)
list(APPEND plugin_libs is_audio_located_bt_node)


foreach(bt_plugin ${plugin_libs})
ament_target_dependencies(${bt_plugin} ${dependencies})
target_compile_definitions(${bt_plugin} PRIVATE BT_PLUGIN_EXPORT)
endforeach()

install(TARGETS
${plugin_libs}
EXPORT ${PROJECT_NAME}
ARCHIVE DESTINATION lib
LIBRARY DESTINATION lib
RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY include/
DESTINATION include/
)


if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_dependencies(${dependencies})
ament_export_targets(${PROJECT_NAME} HAS_LIBRARY_TARGET)

ament_package()
