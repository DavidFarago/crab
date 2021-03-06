# Create libraries  that contain all the crab stuff
add_library(IntraCrab ${CRAB_LIBS_TYPE} crab.cpp)
target_link_libraries(IntraCrab ${CRAB_LIBS})
add_library(InterCrab ${CRAB_LIBS_TYPE} icrab.cpp)
target_link_libraries(InterCrab ${CRAB_LIBS})

function (AddTest testSrc dirName)
  #Extract the filename without an extension (NAME_WE)
  get_filename_component(testName ${testSrc} NAME_WE)
  #Add compile target
  add_executable(${testName} ${testSrc})
  #link with crab libraries
  if (${dirName} MATCHES "inter")
    target_link_libraries(${testName} ${Boost_PROGRAM_OPTIONS_LIBRARY} InterCrab)
  else ()
    target_link_libraries(${testName} ${Boost_PROGRAM_OPTIONS_LIBRARY} IntraCrab)
  endif ()
  #move testing binaries into a test-bin directory
  set_target_properties(${testName} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/test-bin)
  #Finally add it to test execution
  add_test(NAME ${testName} 
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test-bin
    COMMAND ${CMAKE_BINARY_DIR}/test-bin/${testName} )
endfunction()

function (AddTestDir dirName)
  file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${dirName}/*.cc)
  foreach(testSrc ${TEST_SRCS})
    AddTest (${testSrc} ${dirName})
  endforeach(testSrc)
endfunction ()  

AddTestDir (domains)
AddTestDir (inter)
AddTestDir (cg)
AddTestDir (cfg)
AddTestDir (thresholds)
AddTestDir (checkers)
