
CXX =g++
CXXFLAGS = -O2 `pkg-config gtkmm-2.4 gdkmm-2.4 gtkglextmm-1.2 --cflags` -Wall -g

LDFLAGS = `pkg-config --libs gtkmm-2.4 gdkmm-2.4 gtkglextmm-1.2` -g

all: wavevis

wavevis: wavevis.o spikewaveview.o glconfig.o ttreader.o

spikevis: spikevis.o clusterview.o shaders.o glconfig.o

clean:
	rm *.o test
