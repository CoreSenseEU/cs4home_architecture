cmake_minimum_required(VERSION 3.10)
project(cs4home_sound_project)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(cs4home_core REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)
find_package(sound_msgs REQUIRED)
find_package(audio_common_msgs REQUIRED)
find_package(yolo_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sound_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(bt_nodes REQUIRED)


set(dependencies
  ament_cmake
  rclcpp
  rclcpp_lifecycle
  rclcpp_action
  behaviortree_cpp_v3
  cs4home_core
  sound_msgs
  audio_common_msgs
  geometry_msgs
  sound_msgs
  tf2
  tf2_ros
  tf2_geometry_msgs
  bt_nodes
  yolo_msgs
  std_msgs
  visualization_msgs
)

set(SOURCES
  src/${PROJECT_NAME}/SoundCognitiveModule.cpp
  src/${PROJECT_NAME}/SoundSupervisionCognitiveModule.cpp
  src/${PROJECT_NAME}/SoundContextEvaluationCognitiveModule.cpp
  src/${PROJECT_NAME}/HumanAssistanceCognitiveModule.cpp
  src/${PROJECT_NAME}/EmergencyCognitiveModule.cpp
  src/${PROJECT_NAME}/Master.cpp
)

install(DIRECTORY
  config
  DESTINATION share/${PROJECT_NAME}
)

# add_library(${PROJECT_NAME} SHARED
#   src/SoundCognitiveModule.cpp
# )

# ament_target_dependencies(${PROJECT_NAME} ${dependencies})

# CORE
add_library(sound_recognition SHARED src/SoundRecognition.cpp)
ament_target_dependencies(sound_recognition ${dependencies} ${test_dependencies})
target_link_libraries(sound_recognition)

# AFFERENT
add_library(audio_input SHARED src/AudioInput.cpp)
ament_target_dependencies(audio_input ${dependencies} ${test_dependencies})
target_link_libraries(audio_input)

# EFFERENT
add_library(audio_output SHARED src/AudioOutput.cpp)
ament_target_dependencies(audio_output ${dependencies} ${test_dependencies})
target_link_libraries(audio_output)

# META
add_library(audio_meta SHARED src/AudioMeta.cpp)
ament_target_dependencies(audio_meta ${dependencies} ${test_dependencies})
target_link_libraries(audio_meta)

# COUPLING
add_library(audio_coupling SHARED src/AudioCoupling.cpp)
ament_target_dependencies(audio_coupling ${dependencies} ${test_dependencies})
target_link_libraries(audio_coupling)

add_executable(sound_cognitive_module src/SoundCognitiveModule.cpp)
ament_target_dependencies(sound_cognitive_module ${dependencies})
target_link_libraries(sound_cognitive_module sound_recognition audio_input audio_output audio_meta audio_coupling)

# SOUND SUPERVISION COGNITIVE MODULE

add_library(sound_supervision_bt SHARED src/SoundSupervisionBT.cpp)
ament_target_dependencies(sound_supervision_bt ${dependencies} ${test_dependencies})
target_link_libraries(sound_supervision_bt bt_nodes::is_sound_detected_bt_node bt_nodes::is_audio_located_bt_node bt_nodes::rotate_bt_node)

add_executable(sound_supervision_cognitive_module src/SoundSupervisionCognitiveModule.cpp)
ament_target_dependencies(sound_supervision_cognitive_module ${dependencies})
target_link_libraries(sound_supervision_cognitive_module sound_supervision_bt)

# SOUND CONTEXT EVALUATION COGNITIVE MODULE

# CORE

add_library(sound_context_evaluation SHARED src/SoundContextEvaluation.cpp)
ament_target_dependencies(sound_context_evaluation ${dependencies} ${test_dependencies})
target_link_libraries(sound_context_evaluation)

# AFFERENT

add_library(sound_input SHARED src/SoundInput.cpp)
ament_target_dependencies(sound_input ${dependencies} ${test_dependencies})
target_link_libraries(sound_input)

# EFFERENT

add_library(sound_context_output SHARED src/SoundContextOutput.cpp)
ament_target_dependencies(sound_context_output ${dependencies} ${test_dependencies})
target_link_libraries(sound_context_output)

add_executable(sound_context_evaluation_cognitive_module src/SoundContextEvaluationCognitiveModule.cpp)
ament_target_dependencies(sound_context_evaluation_cognitive_module ${dependencies})
target_link_libraries(sound_context_evaluation_cognitive_module sound_context_evaluation sound_input sound_context_output)

# HUMAN ASSISTANCE COGNITIVE MODULE

add_library(human_assistance_bt SHARED src/HumanAssistanceBT.cpp)
ament_target_dependencies(human_assistance_bt ${dependencies} ${test_dependencies})
target_link_libraries(human_assistance_bt bt_nodes::attend_bt_node)

add_executable(human_assistance_cognitive_module src/HumanAssistanceCognitiveModule.cpp)
ament_target_dependencies(human_assistance_cognitive_module ${dependencies})
target_link_libraries(human_assistance_cognitive_module human_assistance_bt)

# EMERGENCY ACTION COGNITIVE MODULE

add_library(emergency_bt SHARED src/EmergencyBT.cpp)
ament_target_dependencies(emergency_bt ${dependencies} ${test_dependencies})
target_link_libraries(emergency_bt bt_nodes::attend_bt_node bt_nodes::is_human_detected_bt_node bt_nodes::rotate_bt_node bt_nodes::is_sound_detected_bt_node)

add_executable(emergency_cognitive_module src/EmergencyCognitiveModule.cpp)
ament_target_dependencies(emergency_cognitive_module ${dependencies})
target_link_libraries(emergency_cognitive_module emergency_bt)

# MASTER

add_executable(sound_master src/Master.cpp)
ament_target_dependencies(sound_master ${dependencies})
target_link_libraries(sound_master)

install(TARGETS
  sound_cognitive_module
  sound_recognition
  audio_input
  audio_output
  audio_meta
  audio_coupling
  sound_supervision_cognitive_module
  sound_supervision_bt
  sound_context_evaluation
  sound_input
  sound_context_output
  sound_context_evaluation_cognitive_module
  human_assistance_bt
  human_assistance_cognitive_module
  emergency_bt
  emergency_cognitive_module
  sound_master
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  DESTINATION lib/${PROJECT_NAME}
)

install(TARGETS
  sound_cognitive_module
  sound_supervision_cognitive_module
  sound_context_evaluation_cognitive_module
  human_assistance_cognitive_module
  emergency_cognitive_module
  sound_master
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY
  launch
  DESTINATION share/${PROJECT_NAME}/
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_export_include_directories(include)
ament_export_libraries(sound_recognition audio_input audio_output audio_meta audio_coupling sound_supervision_bt sound_context_evaluation sound_input sound_context_output human_assistance_bt emergency_bt)
ament_export_dependencies(${dependencies})
ament_package()
