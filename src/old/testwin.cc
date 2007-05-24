#include <iostream>
#include "spikes.h"
#include "FrameCache.h"
#include "ttreader.h"
#include <sys/time.h>
#include <time.h>
#include "ClusterDisplay.h"
#include <gtkmm.h>
#include <gdkmm.h>
#include <gdkmm/pixbuf.h>
#include "SpikeWindow.h"
#include "clusterarea.h"

using namespace std; 

double timeDelta(timeval t1, timeval t2) 
{
  return double(t2.tv_sec * 1e6 + t2.tv_usec - t1.tv_sec*1e6 - t1.tv_usec)/1e6; 
}


void loadSpikePointList (SpikePointList & test, string filename) {
  
  ttreader tt(filename); 

  int spcnt =0; 
  while (! tt.eof() and spcnt < 1000000) {

    SpikePoint sp;
    ttspike spike; 
    int x = tt.getSpike(&spike); 
    
    tt.getPeaks(&spike, &sp); 
    spcnt++; 
    test.push_back(sp);
  }

  // remove all timestamps; 
  unsigned int tsos = test[0].ts;
  cout << "Timestamp offset is " << tsos 
       << " with the last ts being " << test[test.size() -1].ts << endl;
  cout << "This is "
       << (test.back().ts - test.front().ts) / 1e4 /60/60
       << " hours" << endl; 
  for (int i = 0; i < test.size(); i++) {
    test[i].ts = test[i].ts - tsos; 
  }


  timeval t1, t2, t3; 
  gettimeofday(&t1, NULL); 


  
}

void testfun() {
  cout << "testing" << endl; 
}

int main(int argc, char * argv[]) {

  Gtk::Main kit(argc, argv);


  SpikePointList spl; 
  loadSpikePointList (spl, "d118.tt"); 

  SpikeWindow win(&spl); 
  win.show_all_children(); 
  Gtk::Main::run(win);

}

