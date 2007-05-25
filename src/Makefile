
CXX =g++
CXXFLAGS = -O2 `pkg-config gtkmm-2.4 gdkmm-2.4 gtkglextmm-1.2 --cflags` -g

LDFLAGS = `pkg-config --libs gtkmm-2.4 gdkmm-2.4 gtkglextmm-1.2` -g -lboost_thread -lboost_date_time -lboost_program_options -lboost_regex

all: tspikemain spikevis ratetimelinevis



tspikemain: tspikemain.o tspikewin.o network/network.o network/datareceiver.o network/eventreceiver.o  clusterview.o shaders.o glconfig.o  spikewaveview.o glconfig.o ttreader.o network/fakenetwork.o

wavevis: wavevis.o spikewaveview.o glconfig.o ttreader.o

spikevis: spikevis.o clusterview.o shaders.o glconfig.o

ratetimelinevis: ratetimelinevis.o ratetimeline.o

clean:
	rm *.o test
