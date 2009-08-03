#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "tspikewin.h"
#include <somanetwork/fakenetwork.h>
#include <somanetwork/network.h>
#include <somanetwork/logging.h>
#include <somadspio/logging.h>

#include "ttreader.h"
#include <sigc++/sigc++.h>

#include "logging.h" 
#include "tslogging.h"
#include "fakedata.h"

namespace po = boost::program_options;
namespace bf = boost::filesystem;
namespace bl = boost::logging; 

int main(int argc, char** argv)
{
  using namespace std; 

  Gtk::Main kit(argc, argv);

  Gtk::GL::init(argc, argv);
  
  // now our own command options

  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "produce help message")
    ("datasrc", po::value<int>(), "Network data source")
    ("soma-ip", po::value<std::string>(), "Soma IP address")
    ("domain-socket-dir", po::value<string>(), "Domain socket directory for use with testing; use in place of IP")
    ("enable-tspikes-log", po::value<string>(), "Enable logging for tspikes")
    ("enable-network-log", po::value<string>()->default_value("warning"), "Enable soma network debugging at this level")
    ("enable-dspio-log", po::value<string>()->default_value("warning"), "Enable soma DSP IO debugging at this level")
    ;
  

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);    

  if (vm.count("help") or argc == 1) {
    std::cout << desc << "\n";
    return 1;
  }
  
  if (vm.count("enable-network-log")) {
    string logstr = vm["enable-network-log"].as<string>(); 
    if (logstr == "") {
      logstr = "warning"; 
    }
    boost::logging::level::type lt = log_level_parse(logstr); 
    somanetwork::init_logs(lt);  
  }

  if (vm.count("enable-dspio-log")) {
    string logstr = vm["enable-dspio-log"].as<string>(); 
    if (logstr == "") {
      logstr = "warning"; 
    }
    boost::logging::level::type lt = log_level_parse(logstr); 
    somadspio::init_logs(lt);
  }

  if (vm.count("enable-tspikes-log")) {
    string logstr = vm["enable-tspikes-log"].as<string>(); 
    if (logstr == "") {
      logstr = "warning"; 
    }
    boost::logging::level::type lt = log_level_parse(logstr); 
    init_tslogs(lt); 
  }


  somanetwork::pNetworkInterface_t network; 
  
  if (vm.count("soma-ip")) {
    std::string somaip = vm["soma-ip"].as<string>();     
    TSL_(info) << "TSpikeMain: Using IP network to talk to Soma, IP="
	       << somaip; 
    network = somanetwork::Network::createINet(somaip);

  } else if (vm.count("domain-socket-dir")) {
    bf::path domainsockdir(vm["domain-socket-dir"].as<string>()); 
    TSL_(info) << "TSpikeMain: Using Domain Sockets to talk to local process, dir ="
	       << domainsockdir; 

    network = somanetwork::Network::createDomain(domainsockdir); 

  } else {
    TSL_(fatal) << "Soma IP Not Specified, domain socket not specified -- no way to get data" ; 
    return -1; 
  }

  if (!vm.count("datasrc")) {
    TSL_(fatal) << "TSpikeMain: Need to specify a data source (0-63) to listen to"; 

    return -1; 
  }
  
  somatime_t expStartTime = 0; 

  //  fakedata::Grid g(0, 10, 10000); 

  std::vector<TSpike_t> preload_spikes; 
// //   for (int i = 0; i < 50000; i++) {b
// //     preload_spikes.push_back(g.next()); 
// //   }
  
  TSpikeWin tspikewin(network, vm["datasrc"].as<int>(), expStartTime,
 		      preload_spikes);
  
  
  kit.run(tspikewin);
  std::cout << "The end of main()" << std::endl; 
  return 0; 

}
