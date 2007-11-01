#include <boost/program_options.hpp>

#include "tspikewin.h"
#include <somanetwork/fakenetwork.h>
#include <somanetwork/network.h>

#include "ttreader.h"
#include <sigc++/sigc++.h>



class FakeTTData
{
 public:
  FakeTTData(std::string filename, int rate, 
	     FakeNetwork* pni); 
  DataPacket_t *  getSpikeDataPacket(); 
  bool appendToFakeNetwork(FakeNetwork* fn);
  std::vector<TSpike_t>  getManySpikes(int n); 
  void setTime(FakeNetwork * fn, unsigned long usec);

 private:
  ttreader ttreader_; 
  int rate_; 
  Glib::Timer timer_; 
  int spikeNum_;
  FakeNetwork * pni_; 
  void eventTXCallback(const EventTXList_t & el); 
  std::vector<TSpikeChannelState> fakestate_; 
  Event_t createStateResponse(int chan, STATEPARM param);
};

FakeTTData::FakeTTData(std::string filename, int rate, 
		       FakeNetwork* pni):
  ttreader_(filename.c_str()), 
  rate_(rate),
  spikeNum_(0), 
  timer_(), 
  pni_(pni), 
  fakestate_(4)
{

  timer_.start(); 
  // glib interaction to get fake events and make
  // fake responses
  //   Glib::signal_io().connect(sigc::mem_fun(*this, &FakeTTData::eventRXCallback), 
  // 			    pni_->getEventFifoPipe(), Glib::IO_IN); 
  pni_->signalEventTX().connect(sigc::mem_fun(*this, &FakeTTData::eventTXCallback)); 
  				
  
  // set up the fake state
  for (int i = 0; i < 4; i ++) {
    fakestate_[i].gain = 100; 
    fakestate_[i].threshold = 70000; 
    fakestate_[i].hpf = true; 
    fakestate_[i].filt = 0x01; 
    fakestate_[i].rangeMin = -500000; 
    fakestate_[i].rangeMax = 500000; 
  }

}

Event_t FakeTTData::createStateResponse(int chan, STATEPARM parm)
{

  Event_t response; 
  response.src = dsrc_to_esrc(0); // DEBUGGING; WE NEED TO FIGURE OUT WHO THE HELL WE ARE
  response.cmd = 0x92; 
  
  // right now we just support gain; 
  switch(parm) {
  case GAIN:
    response.data[0] = (GAIN << 8) |  chan ;
    response.data[1] = fakestate_[chan].gain; 
    break; 

  case RANGE:
    {
      
    response.data[0] = (RANGE << 8) |  chan ;
    unsigned short x1 = (fakestate_[chan].rangeMin >> 16) & 0xFFFF; 
    unsigned short x2  = (fakestate_[chan].rangeMin & 0xFFFF); 
    response.data[1] = x1; 
    response.data[2] = x2; 
    response.data[3] = (fakestate_[chan].rangeMax >> 16) & 0xFFFF;  
    response.data[4] = (fakestate_[chan].rangeMax & 0xFFFF); 

    break; 
    }
  default:
    break; 
  }
  return response; 
}

void FakeTTData::eventTXCallback(const EventTXList_t & el)
{
  // extract out events and create responses
  EventList_t * pelist = new EventList_t(); 
  pelist->reserve(el.size()); 

  EventTXList_t::const_iterator e; 
  for ( e = el.begin(); e != el.end(); e++) {
    Event_t evt = e->event; 
    
    switch(evt.cmd) {
      
    case 0x90:  { // SET VALUE
      
      int chan = evt.data[0] & 0xFF; 
      STATEPARM cmd = toStateParm((evt.data[0] >> 8) & 0xFF); 
      
      switch (cmd) {
      case GAIN : { 
	fakestate_[chan].gain = evt.data[1]; 
	fakestate_[chan].rangeMin = -2048000 / fakestate_[chan].gain; 
	fakestate_[chan].rangeMax = 2048000 / fakestate_[chan].gain; 
	std::cout << "we have just set the ranges to" 
		  << fakestate_[chan].rangeMin << " " 
		  << fakestate_[chan].rangeMax << std::endl; 

	Event_t e1 = createStateResponse(chan, GAIN); 
	Event_t e2 = createStateResponse(chan, RANGE); 

	pelist->push_back(e1); 
	pelist->push_back(e2); 
	break; 
      }
      default:
	break; 
      }
      

      break; 
    }
    case 0x91: { // QUERY
      // Query Source State
      int chan = evt.data[0]; 
      STATEPARM cmd = toStateParm(evt.data[1]); 
      Event_t e = createStateResponse(chan, cmd); 
      pelist->push_back(e); 
      
      break; 
      
    }
    }
  }
  pni_->appendEventOut(pelist); 
  
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

  if (numtotx > 0 )
    {
      for (int i = 0; i < numtotx; i++) {
	TSpike_t ts = ttreader_.getTSpike();
	ts.time = ts.time * 5; 
	DataPacket_t *  dp = rawFromTSpike(ts); 
	fn->appendDataOut(dp); 
	setTime(fn, ts.time); 
	
      }
      
      timer_.reset(); 

    }
  
  
  return true; 


}

void FakeTTData::setTime(FakeNetwork * fn, uint64_t usec)
{
  Event_t event; 
  event.src = 0; 
  event.cmd = 0x10; 
  event.data[0] = (usec >> 32) & 0xFFFF;
  event.data[1] = (usec >> 16 ) & 0xFFFF; 
  event.data[2] = usec & 0xFFFF; 

  EventList_t * pelt = new EventList_t; 
  pelt->push_back(event); 
  fn->appendEventOut(pelt); 
  
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
    ("networksrc", po::value<int>(), "Network source")
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
  
  if (vm.count("networksrc")) {
    
    Network net("192.168.1.100"); 
    net.enableDataRX( vm["networksrc"].as<int>(), TSPIKE); 
    
    TSpikeWin tspikewin(&net, vm["networksrc"].as<int>());
    
    //net.run(); 
    kit.run(tspikewin);
    
    return 0; 

  } else {
    // ZOMG these abstractions are all wrong; should clean up

    FakeNetwork net; 

    int ttrate = 0; 
    if (vm.count("ttfile")) {
      
      
      if (vm.count("ttrate") )
	{
	  ttrate = vm["ttrate"].as<int>(); 
	  
	}
    }    

    FakeTTData fttd(vm["ttfile"].as<std::string>(), ttrate, &net); 
    
    TSpikeWin tspikewin(&net, 0); 

    if (vm.count("ttprenum") )
      {
	std::vector<TSpike_t> spikes 
	  = fttd.getManySpikes(vm["ttprenum"].as<int>()); 
	
	tspikewin.loadExistingSpikes(spikes); 
      }
    if (vm.count("ttrate") )
      
      {
	// actually run from fake network
	Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(fttd, 
								&FakeTTData::appendToFakeNetwork), &net), 50);
	
	
      }
    net.run(); 
    
    
    kit.run(tspikewin);
  }


  return 0;
}
