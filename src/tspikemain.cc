#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "tspikewin.h"
#include <somanetwork/fakenetwork.h>
#include <somanetwork/network.h>

#include "ttreader.h"
#include <sigc++/sigc++.h>

#include "logging.h" 
#include "fakedata.h"

namespace po = boost::program_options;
namespace bf = boost::filesystem;

std::string LOGROOT("soma.tspikes"); 

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  Gtk::GL::init(argc, argv);
  
  // now our own command options

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("datasrc", po::value<int>(), "Network data source")
    ("soma-ip", po::value<std::string>(), "Soma IP address")
    ("domain-socket-dir", po::value<string>(), "Domain socket directory for use with testing; use in place of IP")
    ;
  
  desc.add(logging_desc()); 

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    
  
  int loglevel = config_logging(vm, LOGROOT); 

  if (vm.count("help") or argc == 1) {
    std::cout << desc << "\n";
    return 1;
  }
  
  log4cpp::Category& logtspike = log4cpp::Category::getInstance(LOGROOT);

  somanetwork::pNetworkInterface_t network; 
  
  if (vm.count("soma-ip")) {
    std::string somaip = vm["soma-ip"].as<string>();     
    logtspike.infoStream() << "Using IP network to talk to soma"; 
    logtspike.infoStream() << "soma hardware IP: " << somaip; 
    network = somanetwork::Network::createINet(somaip);

  } else if (vm.count("domain-socket-dir")) {
    bf::path domainsockdir(vm["domain-socket-dir"].as<string>()); 

    logtspike.infoStream() << "Using domain sockets to talk to local process"; 
    logtspike.infoStream() << "domain socket dir: " << domainsockdir; 
    network = somanetwork::Network::createDomain(domainsockdir); 

  } else {
    logtspike.fatal("soma-ip not specified, domain-socket-dir not specified; no way to get data"); 
    return -1; 
  }

  if (!vm.count("datasrc")) {
    logtspike.fatal("Neet to specify a data source (0-63) to listen to"); 
    return -1; 
  }
  
  somatime_t expStartTime = 0; 
  // the time at which the experiment is viewed as "starting" 

  //  fakedata::Grid g(0, 10, 10000); 

  std::vector<TSpike_t> preload_spikes; 
//   for (int i = 0; i < 50000; i++) {
//     preload_spikes.push_back(g.next()); 
//   }
  
  logtspike.info("Constructing GUI object"); 
  TSpikeWin tspikewin(network, vm["datasrc"].as<int>(), expStartTime,
		      preload_spikes);
  
  logtspike.info("Setting network state to run"); 
  network->run(); 
  kit.run(tspikewin);
  
  return 0; 

}
