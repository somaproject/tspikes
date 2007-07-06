#include <boost/program_options.hpp>

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
  spikeNum_(0), 
  timer_()
{

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

  
  double seconds; 
  unsigned long useconds; 
  seconds = timer_.elapsed(useconds); // number of secs since last call
  int numtotx = int (rate_* seconds); 

  numtotx = 5; 

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

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  Gtk::GL::init(argc, argv);
  
  // now our own command options

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("ttfile", po::value<std::string>(), "ttfile to read in data from")
    ("ttprenum", po::value<int>(), "number of spikes to read from ttfile")
    ("ttrate", po::value<int>(), "rough rate of spike arrival")
    ;
  
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  
  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }
  

  FakeNetwork net; 

  TSpikeWin tspikewin(&net);

  int ttrate = 0; 
  if (vm.count("ttfile")) {
    

    if (vm.count("ttrate") )
      {
	ttrate = vm["ttrate"].as<int>(); 
	
      }
  }    
  FakeTTData fttd(vm["ttfile"].as<std::string>(), ttrate); 
  if (vm.count("ttprenum") )
    {
      
      
	std::vector<TSpike_t> spikes 
	  = fttd.getManySpikes(vm["ttprenum"].as<int>()); 
	
	tspikewin.loadExistingSpikes(spikes); 
    }
  if (vm.count("ttrate") )
    
    {
      // actually run from network
      Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(fttd, 
							      &FakeTTData::appendToFakeNetwork), &net), 50);
      
      
    }

  net.run(); 
  

  kit.run(tspikewin);

  return 0;
}
