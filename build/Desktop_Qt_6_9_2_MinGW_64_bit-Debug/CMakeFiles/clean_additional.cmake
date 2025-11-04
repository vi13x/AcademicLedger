# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\ElectronicJournal_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\ElectronicJournal_autogen.dir\\ParseCache.txt"
  "ElectronicJournal_autogen"
  )
endif()
