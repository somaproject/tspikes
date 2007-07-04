#include "tspikewin.h"
#include <somanetwork/fakenetwork.h>
#include "ttreader.h"
#include <sigc++/sigc++.h>

class FakeTTData
{
 public:
  FakeTTData(std::string filename, int rate); 
  DataPacket_t *  getSpikeDataPacket(); 
  bool appendToFakeNetwork(FakeNetwork* fn);
  std::vector<TSpike_t>  getManySpikes(int n); 

 private:
  ttreader ttreader_; 
  int rate_; 
  Glib::Timer timer_; 
  int spikeNum_;
};

FakeTTData::FakeTTData(std::string filename, int rate):
  ttreader_(filename.c_str()), 
  rate_(rate),
  spikeNum_(0)
{
  
  std::cout << spikeNum_ << std::endl;
  timer_.start(); 

}

std::vector<TSpike_t> FakeTTData::getManySpikes(int n)
{

  std::vector<TSpike_t> spikes;
  spikes.reserve(n); 

  for (int i = 0; i < 100000; i++)
    {
      TSpike_t ts; 
      ts = ttreader_.getTSpike(); 
      spikes.push_back(ts); 
    }

  return spikes; 

}

DataPacket_t * FakeTTData::getSpikeDataPacket()
{
  TSpike_t ts; 
  ts = ttreader_.getTSpike(); 
  spikeNum_++; 

  return rawFromTSpike(ts); 

} 

bool FakeTTData::appendToFakeNetwork(FakeNetwork* fn)
{


  double seconds = timer_.elapsed(); // number of secs since last call
  int numtotx = int (rate_* seconds); 

  
  if (numtotx > 0 )
    {
      for (int i = 0; i < numtotx; i++) {
	TSpike_t ts = ttreader_.getTSpike();
	ts.time = ts.time * 5; 
	DataPacket_t *  dp = rawFromTSpike(ts); 

	fn->appendDataOut(dp); 
      }
      
      timer_.reset(); 

    }
  
  
  return true; 


}


bool fakesettime(TSpikeWin* tsw)
{

  static double time = 0; 
  time += 1.0;
  rtime_t rt(time); 
  //std::cout << "The time is now " << time << std::endl; 
  tsw->setTime(rt); 
  return true; 


}

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  Gtk::GL::init(argc, argv);
  
  FakeNetwork net; 
  FakeTTData fttd("../../d215.tt", 100); 
  
  TSpikeWin tspikewin(&net);
  std::vector<TSpike_t> spikes = fttd.getManySpikes(100000); 
  tspikewin.loadExistingSpikes(spikes); 

  

  kit.run(tspikewin);

  return 0;
}
