# 


AM_LDFLAGS = $(GTKDEPS_LIBS) $(BOOST_LDFLAGS) $(SOMADEPS_LIBS) $(BOOST_THREAD_LIB)  $(BOOST_PROGRAM_OPTIONS_LIB)


h_sources = clusterrenderer.h  glconfig.h  ratetimeline.h  spikewaveview.h \
	 ttreader.h clusterview.h  glspikes.h  shaders.h tspikewin.h \
	 spikewaverenderer.h somanetcodec.h \
	 sourcesettingswin.h gaincombosetting.h fakettdata.h hpfchecksetting.h  \
	 combosetting.h filtercombosetting.h tholdentrysetting.h voltage.h \
	ratetimelinerenderer.h

cc_sources = clusterrenderer.cc glspikes.cc  spikewaveview.cc \
	 ttreader.cc clusterview.cc \
	glconfig.cc  ratetimeline.cc tspikemain.cc \
	 tspikewin.cc spikewaverenderer.cc somanetcodec.cc \
	 sourcesettingswin.cc gaincombosetting.cc fakettdata.cc \
	 hpfchecksetting.cc combosetting.cc filtercombosetting.cc \
	 tholdentrysetting.cc voltage.cc ratetimelinerenderer.cc

ext_sources = ext-common/cairogltext/glstring.cc ext-common/shaderutil/shaders.cc
bin_PROGRAMS = tspike spikevis ratetimelinevis
spikevis_SOURCES =  clusterrenderer.cc  clusterview.cc  glconfig.cc  spikevis.cc  voltage.cc $(ext_sources)

ratetimelinevis_SOURCES = ratetimelinevis.cc ttreader.cc ratetimeline.cc ratetimelinerenderer.cc glspikes.cc glconfig.cc  $(ext_sources)

tspike_SOURCES = $(ext_sources) $(cc_sources) 
tspikedir = $(datadir)/tspikes
tspike_DATA = cluster.frag glstring.frag rtgrad.frag cluster.vert glstring.vert rtgrad.vert tspikes.glade
LDADD = $(BOOST_THREAD_LIBS) $(SOMADEPS_LIBS) $(GTKDEPS_LIBS)  $(BOOST_PROGRAM_OPTIONS_LIBS)
tspike_LDFLAGS = $(BOOST_THREAD_LDFLAGS)  $(AM_LDFLAGS) $(BOOST_PROGRAM_OPTIONS_LDFLAGS)


#http://www.gnu.org/software/autoconf/manual/autoconf-2.57/html_chapter/autoconf_17.html#SEC179  FAQ entry for how to handle the data directory


AM_CXXFLAGS = -DDATADIR=\"$(tspikedir)\"  $(GTKDEPS_CFLAGS)  $(BOOST_CPPFLAGS) $(SOMADEPS_CFLAGS)  -pthread -Iext-common/
