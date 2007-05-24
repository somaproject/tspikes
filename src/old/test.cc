#include <iostream>
#include "spikes.h"
#include "ttreader.h"
#include <sys/time.h>
#include <time.h>
#include "ClusterDisplay.h"
#include <gtkmm.h>
#include <gdkmm.h>
#include <gdkmm/pixbuf.h>

using namespace std; 

double timeDelta(timeval t1, timeval t2) 
{
  return double(t2.tv_sec * 1e6 + t2.tv_usec - t1.tv_sec*1e6 - t1.tv_usec)/1e6; 
}



int main(int argc, char * argv[]) {

  Gtk::Main kit(argc, argv);


  SpikePointList test; 
  
  ttreader tt("d215.tt"); 

  int spcnt =0; 
  while (! tt.eof() and spcnt < 1000000) {

    SpikePoint sp;
    ttspike spike; 
    int x = tt.getSpike(&spike); 
    
    tt.getPeaks(&spike, &sp); 
    spcnt++; 
    test.push_back(sp);
  }


  unsigned int tsos = test[0].ts;
  for (int i = 0; i < test.size(); i++) {
    test[i].ts = test[i].ts - tsos; 
  }


  timeval t1, t2, t3; 
  gettimeofday(&t1, NULL); 

  SpikeView sv(0, 0, 200, 200, 1000.0, 3, 4); 
  
  SpikeTimeline tl;
  tl.granularity = 600000; 
  tl.endTime = std::numeric_limits<unsigned int>::max(); 
  tl.rate = 20; 
  cout << "The end is " << test.back().ts << endl; 
  
  ClusterDisplay cd(&test, sv, tl); 
  cd.rebuild(); 
  cd.save();


}

