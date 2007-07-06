
CXX =ccache g++
CXXFLAGS = -O2 `pkg-config gtkmm-2.4 gdkmm-2.4 gtkglextmm-1.2 somanetwork-1.0 --cflags` -g -DOPENGL2

LDFLAGS = `pkg-config --libs gtkmm-2.4 gdkmm-2.4 gtkglextmm-1.2 somanetwork-1.0` -g -lboost_thread -lboost_date_time -lboost_program_options -lboost_regex

all: tspikemain



tspikemain: tspikemain.o tspikewin.o clusterview.o shaders.o glconfig.o  spikewaveview.o glconfig.o ttreader.o ratetimeline.o cairogltext/glstring.o

clean:
	rm *.o 
