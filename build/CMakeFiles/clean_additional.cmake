# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  [[CMakeFiles\FoodDeliveryApp_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\FoodDeliveryApp_autogen.dir\ParseCache.txt]]
  [[CMakeFiles\backend_tests_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\backend_tests_autogen.dir\ParseCache.txt]]
  [[CMakeFiles\ds_core_autogen.dir\AutogenUsed.txt]]
  [[CMakeFiles\ds_core_autogen.dir\ParseCache.txt]]
  "FoodDeliveryApp_autogen"
  "backend_tests_autogen"
  "ds_core_autogen"
  )
endif()
