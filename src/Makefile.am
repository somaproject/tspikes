# 


AM_LDFLAGS = $(GTKDEPS_LIBS) $(BOOST_LDFLAGS) $(SOMADEPS_LIBS) $(BOOST_THREAD_LIB)  $(BOOST_PROGRAM_OPTIONS_LIB)


h_sources = clusterrenderer.h  glconfig.h  ratetimeline.h  spikewaveview.h \
	 ttreader.h clusterview.h  glspikes.h  shaders.h tspikewin.h \
	 cairogltext/glstring.h spikewaverenderer.h somanetcodec.h \
	 sourcesettingswin.h gaincombosetting.h fakettdata.h hpfchecksetting.h  \
	 combosetting.h filtercombosetting.h tholdentrysetting.h voltage.h \
	ratetimelinerenderer.h

cc_sources = clusterrenderer.cc glspikes.cc  spikewaveview.cc \
	 cairogltext/glstring.cc ttreader.cc clusterview.cc \
	 shaders.cc  glconfig.cc  ratetimeline.cc tspikemain.cc \
	 tspikewin.cc spikewaverenderer.cc somanetcodec.cc \
	 sourcesettingswin.cc gaincombosetting.cc fakettdata.cc \
	 hpfchecksetting.cc combosetting.cc filtercombosetting.cc \
	 tholdentrysetting.cc voltage.cc ratetimelinerenderer.cc


bin_PROGRAMS = tspike spikevis ratetimelinevis
spikevis_SOURCES =  clusterrenderer.cc  clusterview.cc  shaders.cc  glconfig.cc  cairogltext/glstring.cc  spikevis.cc  voltage.cc

ratetimelinevis_SOURCES = ratetimelinevis.cc ttreader.cc ratetimeline.cc ratetimelinerenderer.cc \
	 glspikes.cc glconfig.cc  shaders.cc cairogltext/glstring.cc 

tspike_SOURCES = $(cc_sources)
tspikedir = $(datadir)/tspikes
tspike_DATA = cluster.frag glstring.frag rtgrad.frag cluster.vert glstring.vert rtgrad.vert tspikes.glade


#http://www.gnu.org/software/autoconf/manual/autoconf-2.57/html_chapter/autoconf_17.html#SEC179  FAQ entry for how to handle the data directory


AM_CXXFLAGS = -DDATADIR=\"$(tspikedir)\"  $(GTKDEPS_CFLAGS)  $(BOOST_CPPFLAGS) $(SOMADEPS_CFLAGS)  -pthread 
