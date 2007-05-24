
CXX =g++
CXXFLAGS = -O2 `pkg-config gtkmm-2.4 gdkmm-2.4 gtkglextmm-1.2 --cflags` -Wall

LDFLAGS = `pkg-config --libs gtkmm-2.4 gdkmm-2.4 gtkglextmm-1.2`

all: spikevis vistest

vistest: vistest.o ttreader.o

spikevis: spikevis.o

clean:
	rm *.o test
