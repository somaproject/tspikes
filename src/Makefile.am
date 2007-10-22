# 

AM_CXXFLAGS = $(GTKDEPS_CFLAGS)  $(BOOST_CPPFLAGS) $(SOMADEPS_CFLAGS)  -pthread 
AM_LDFLAGS = $(GTKDEPS_LIBS) $(BOOST_LDFLAGS) $(SOMADEPS_LIBS) $(BOOST_THREAD_LIB)  $(BOOST_PROGRAM_OPTIONS_LIB)


h_sources = clusterrenderer.h  glconfig.h  ratetimeline.h  spikewaveview.h  ttreader.h clusterview.h  glspikes.h  shaders.h tspikewin.h cairogltext/glstring.h spikewaverenderer.h

cc_sources = clusterrenderer.cc  spikewaveview.cc  cairogltext/glstring.cc ttreader.cc clusterview.cc  shaders.cc  glconfig.cc  ratetimeline.cc tspikemain.cc tspikewin.cc spikewaverenderer.cc

bin_PROGRAMS = tspike spikevis
spikevis_SOURCES =  clusterrenderer.cc  clusterview.cc  shaders.cc  glconfig.cc  cairogltext/glstring.cc  spikevis.cc

tspike_SOURCES = $(cc_sources)

