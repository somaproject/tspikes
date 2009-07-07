
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GUIDEPS_INCLUDE_DIRS})
include_directories(${SOMADEPS_INCLUDE_DIRS})


link_directories(
  ${Boost_LIBRARY_DIRS}
  ${GUIDEPS_LIBRARY_DIRS}
  ${SOMADEPS_LIBRARY_DIRS}
)

add_executable(spvectdb_test
  runner.cc
  spvectdb_test.cc
)

add_executable(util_test
  runner.cc
  util_test.cc
)

set(testlist 
  spvectdb_test
  util_test
)

foreach(testname ${testlist})
  
  target_link_libraries(${testname}
    spvectdb
    ${Boost_LIBRARIES} 
    ${EXPDEPS_LIBRARIES}
    )
  
  ADD_TEST(${testname} ${testname})

endforeach(testname)

