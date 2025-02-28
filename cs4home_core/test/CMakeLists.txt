find_package(sensor_msgs REQUIRED)
find_package(vision_msgs REQUIRED)
find_package(ament_index_cpp REQUIRED)

set(test_dependencies
  sensor_msgs
  vision_msgs
  ament_index_cpp
)

add_library(image_filter SHARED ImageFilter.cpp)
ament_target_dependencies(image_filter ${dependencies} ${test_dependencies})
target_link_libraries(image_filter ${PROJECT_NAME})

add_library(image_filter_cb SHARED ImageFilterCB.cpp)
ament_target_dependencies(image_filter_cb ${dependencies} ${test_dependencies})
target_link_libraries(image_filter_cb ${PROJECT_NAME})

add_library(simple_image_input SHARED SimpleImageInput.cpp)
ament_target_dependencies(simple_image_input ${dependencies} ${test_dependencies})
target_link_libraries(simple_image_input ${PROJECT_NAME})

add_library(simple_image_output SHARED SimpleImageOutput.cpp)
ament_target_dependencies(simple_image_output ${dependencies} ${test_dependencies})
target_link_libraries(simple_image_output ${PROJECT_NAME})

add_library(default_meta SHARED DefaultMeta.cpp)
ament_target_dependencies(default_meta ${dependencies} ${test_dependencies})
target_link_libraries(default_meta ${PROJECT_NAME})

add_library(default_coupling SHARED DefaultCoupling.cpp)
ament_target_dependencies(default_coupling ${dependencies} ${test_dependencies})
target_link_libraries(default_coupling ${PROJECT_NAME})


install(TARGETS
  image_filter
  image_filter_cb
  simple_image_input
  simple_image_output
  default_meta
  default_coupling
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib/${PROJECT_NAME}
)

ament_add_gtest(cognitive_module_test cognitive_module_test.cpp)
target_link_libraries(cognitive_module_test ${PROJECT_NAME})
ament_target_dependencies(cognitive_module_test ${test_dependencies})

ament_add_gtest(master_test master_test.cpp)
target_link_libraries(master_test ${PROJECT_NAME})
ament_target_dependencies(master_test ${test_dependencies})


install(DIRECTORY
  config
  DESTINATION share/${PROJECT_NAME}
)
