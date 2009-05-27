set(tspikes_SRCS
  clusterrenderer.cc glspikes.cc  spikewaveview.cc
  ttreader.cc clusterview.cc
  glconfig.cc  ratetimeline.cc tspikemain.cc
  tspikewin.cc spikewaverenderer.cc somanetcodec.cc
  sourcesettingswin.cc gaincombosetting.cc 
  hpfchecksetting.cc combosetting.cc filtercombosetting.cc
  tholdentrysetting.cc voltage.cc ratetimelinerenderer.cc )


set(ext_SRCS
  ext-common/cairogltext/glstring.cc
  ext-common/shaderutil/shaders.cc
)

set(logging_SRCS
  ext-logging/src/logging.cc
)

set(clustervis_SRCS
  clusterrenderer.cc  
  clusterview.cc 
  glconfig.cc  
  spikevis.cc  
  voltage.cc)

set(SHADER_PATH 
  ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME})

set(GLADE_PATH 
  ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME})

configure_file(config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GUIDEPS_INCLUDE_DIRS})
include_directories(${SOMADEPS_INCLUDE_DIRS})
include_directories(ext-common/)
include_directories(ext-logging/src)


ADD_EXECUTABLE(tspikes
  spvectdb.cc
  ${tspikes_SRCS}
  ${ext_SRCS}
  ${logging_SRCS}
)

ADD_LIBRARY(spvectdb
  spvectdb.cc
  ${tspikes_SRCS}
  ${ext_SRCS}
  ${logging_SRCS}
)

TARGET_LINK_LIBRARIES(spvectdb
  ${Boost_LIBRARIES} 
  ${GUIDEPS_LIBRARIES}
  ${SOMADEPS_LIBRARIES}
)

TARGET_LINK_LIBRARIES(tspikes
  ${Boost_LIBRARIES} 
  ${GUIDEPS_LIBRARIES}
  ${SOMADEPS_LIBRARIES}
)


ADD_EXECUTABLE(tspikes-test-clustervis
  ${clustervis_SRCS}
  ${ext_SRCS}
)

TARGET_LINK_LIBRARIES(tspikes-test-clustervis
  ${Boost_LIBRARIES} 
  ${GUIDEPS_LIBRARIES}
  ${SOMADEPS_LIBRARIES}
)

set(SHADERS 
  cluster.vert cluster.frag
  glstring.vert glstring.frag
  rtgrad.vert rtgrad.frag)

set(GLADEFILES
  tspikes.glade)

install( TARGETS tspikes tspikes-test-clustervis
  DESTINATION bin)

install(FILES ${SHADERS}
  DESTINATION share/${PROJECT_NAME})

install(FILES ${GLADEFILES}
  DESTINATION share/${PROJECT_NAME})





