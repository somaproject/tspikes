#include <boost/program_options.hpp>

#include "tspikewin.h"
#include <somanetwork/fakenetwork.h>
#include <somanetwork/network.h>

#include "ttreader.h"
#include <sigc++/sigc++.h>

#include "fakettdata.h" 

namespace po = boost::program_options;

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  Gtk::GL::init(argc, argv);
  
  // now our own command options

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("datasrc", po::value<int>(), "Network data source")
    ("somaip", po::value<std::string>(), "Soma IP address")
    ("ttfile", po::value<std::string>(), "ttfile to read in data from")
    ("ttprenum", po::value<int>(), "number of spikes to read from ttfile")
    ("ttrate", po::value<int>(), "rough rate of spike arrival")
    ;
  
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  
  if (vm.count("help") or argc == 1) {
    std::cout << desc << "\n";
    return 1;
  }
  
  if (vm.count("datasrc") and vm.count("ttfile")) {
    std::cout << "Cannot run with both file and network" << std::endl; 
    return 1; 
  }
  if (vm.count("datasrc")) {
    
    if(!vm.count("somaip")) {
      std::cout << "Cannot run without specified IP address" << std::endl; 

    }
    Network net(vm["somaip"].as<std::string>()); 
    net.enableDataRX( vm["datasrc"].as<int>(), TSPIKE); 
    
    TSpikeWin tspikewin(&net, vm["datasrc"].as<int>());
    
    net.run(); 
    kit.run(tspikewin);
    
    return 0; 

  } else {
    // ZOMG these abstractions are all wrong; should clean up

    FakeNetwork net; 
    net.enableDataRX(0, TSPIKE); 

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
	if (ttrate > 0 ) {
	Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(fttd, 
								&FakeTTData::appendToFakeNetwork), &net), 50);
	} else { 
	Glib::signal_timeout().connect(sigc::bind(sigc::mem_fun(fttd, 
								&FakeTTData::appendToFakeNetworkWithRealTime), &net), 50);
	}
	
      }
    net.run(); 
    
    
    kit.run(tspikewin);
  }


  return 0;
}
