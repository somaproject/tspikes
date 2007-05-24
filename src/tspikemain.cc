#include "tspikewin.h"
#include "network/fakenetwork.h"
#include "ttreader.h"
#include <sigc++/sigc++.h>

class FakeTTData
{
 public:
  FakeTTData(std::string filename, int rate); 
  DataPacket_t *  getSpikeDataPacket(); 
  bool appendToFakeNetwork(FakeNetwork* fn);

 private:
  ttreader ttreader_; 
  int rate_; 
  Glib::Timer timer_; 

};

FakeTTData::FakeTTData(std::string filename, int rate):
  ttreader_(filename.c_str()), 
  rate_(rate)
{

  timer_.start(); 
}

DataPacket_t * FakeTTData::getSpikeDataPacket()
{
  TSpike_t ts; 
  ts.src = 0; 
  ts.time = 100; 
  ts.x.filtid = 1; 
  ts.y.filtid = 2; 
  ts.a.filtid = 3; 
  ts.b.filtid = 4; 
  
  ts.x.valid = 1; 
  ts.y.valid = 1; 
  ts.a.valid = 1; 
  ts.b.valid = 1; 
  
  ts.x.threshold = 1000; 
  ts.y.threshold = 2000; 
  ts.a.threshold = 3000; 
  ts.b.threshold = 4000; 
  
  
  ttspike tts; 
  ttreader_.getSpike(&tts); 
  // copy
  for (int i = 0; i < 32; i++)
    {
      ts.x.wave[i] = tts.w1[i]; 
      ts.y.wave[i] = tts.w2[i]; 
      ts.a.wave[i] = tts.w3[i]; 
      ts.b.wave[i] = tts.w4[i]; 
    }

  return rawFromTSpike(ts); 

} 

bool FakeTTData::appendToFakeNetwork(FakeNetwork* fn)
{

  double seconds = timer_.elapsed(); // number of secs since last call
  int numtotx = int (rate_* seconds); 
  if (numtotx > 0 )
    {
      for (int i = 0; i < numtotx; i++) {
	fn->appendDataOut(getSpikeDataPacket()); 
      }
      
      timer_.reset(); 
    }
  
  return true; 


}

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  Gtk::GL::init(argc, argv);
  
  FakeNetwork net; 
  FakeTTData fttd("../../d118.tt", 100); 
  
  Glib::signal_timeout().connect(
				 sigc::bind(sigc::mem_fun(&fttd, 
							  &FakeTTData::appendToFakeNetwork), 
					    &net),
				 10); 
  TSpikeWin tspikewin(&net);
  
  kit.run(tspikewin);

  return 0;
}
