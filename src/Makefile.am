# 

AM_CXXFLAGS = $(GTKDEPS_CFLAGS)  $(BOOST_CPPFLAGS) -pthread 
AM_LDFLAGS = $(GTKDEPS_LIBS) $(BOOST_LDFLAGS) $(BOOST_THREAD_LIB)


h_sources = clusterrenderer.h  glconfig.h  ratetimeline.h  spikewaveview.h  ttreader.h clusterview.h  glspikes.h  shaders.h tspikewin.h

cc_sources = clusterrenderer.cc  ratetimelinevis.cc  spikewaveview.cc  ttreader.cc clusterview.cc  shaders.cc test.cc glconfig.cc  spikevis.cc tspikemain.cc ratetimeline.cc tspikewin.cc

bin_PROGRAMS = tspike
tspike_SOURCES = $(cc_sources)

