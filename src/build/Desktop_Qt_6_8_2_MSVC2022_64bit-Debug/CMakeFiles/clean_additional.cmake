# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\FirstQt_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\FirstQt_autogen.dir\\ParseCache.txt"
  "FirstQt_autogen"
  )
endif()
