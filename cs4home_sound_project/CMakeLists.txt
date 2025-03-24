cmake_minimum_required(VERSION 3.8)
project(cs4home_sound_project)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(cs4home_core REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sound_msgs REQUIRED)
find_package(audio_common_msgs REQUIRED)
# find_package(tensorflow_ros_cpp REQUIRED)

set(dependencies
  ament_cmake
  rclcpp
  rclcpp_lifecycle
  rclcpp_lifecycle
  cs4home_core
  sound_msgs
  audio_common_msgs
  # tensorflow_ros_cpp
)

set(SOURCES
  src/${PROJECT_NAME}/SoundCognitiveModule.cpp
)

install(DIRECTORY
  config
  DESTINATION share/${PROJECT_NAME}
)

add_library(${PROJECT_NAME} SHARED
  src/SoundCognitiveModule.cpp
)
ament_target_dependencies(${PROJECT_NAME} ${dependencies})

add_executable(sound_cognitive_module src/SoundCognitiveModule.cpp)
ament_target_dependencies(sound_cognitive_module ${dependencies})
target_link_libraries(sound_cognitive_module ${PROJECT_NAME})

# CORE
add_library(sound_recognition SHARED src/SoundRecognition.cpp)
ament_target_dependencies(sound_recognition ${dependencies} ${test_dependencies})
target_link_libraries(sound_recognition ${PROJECT_NAME})

# AFFERENT
add_library(audio_input SHARED src/AudioInput.cpp)
ament_target_dependencies(audio_input ${dependencies} ${test_dependencies})
target_link_libraries(audio_input ${PROJECT_NAME})

# EFFERENT
add_library(audio_output SHARED src/AudioOutput.cpp)
ament_target_dependencies(audio_output ${dependencies} ${test_dependencies})
target_link_libraries(audio_output ${PROJECT_NAME})

# META
add_library(meta SHARED src/DefaultMeta.cpp)
ament_target_dependencies(meta ${dependencies} ${test_dependencies})
target_link_libraries(meta ${PROJECT_NAME})

# COUPLING
add_library(coupling SHARED src/DefaultCoupling.cpp)
ament_target_dependencies(coupling ${dependencies} ${test_dependencies})
target_link_libraries(coupling ${PROJECT_NAME})

install(TARGETS
  sound_cognitive_module
  sound_recognition
  audio_input
  audio_output
  meta
  coupling
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  DESTINATION lib/${PROJECT_NAME}
)

install(TARGETS
  sound_cognitive_module
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
ament_export_libraries(${PROJECT_NAME})
ament_export_dependencies(${dependencies})
ament_package()
