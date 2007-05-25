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
  ts = ttreader_.getTSpike(); 
  std::cout << ts.x.wave[7] << std::endl;
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
