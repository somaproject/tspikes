#include "tslogging.h"
#include <boost/logging/format.hpp>
#include <boost/logging/format/formatter/tags.hpp>
#include <boost/logging/format/named_write.hpp>
#include <boost/logging/format/formatter/tags.hpp>
#include <boost/logging/format/formatter/high_precision_time.hpp>

  //using namespace boost::logging;
namespace bl = boost::logging; 



BOOST_DEFINE_LOG_FILTER(tspikes_log_level, tspikes_finder::filter ) // holds the application log level
BOOST_DEFINE_LOG(tspikes_l, tspikes_finder::logger); 

void init_tslogs( boost::logging::level::type level)  {
  tspikes_log_level()->set_enabled(level); 

  tspikes_l()->writer().add_formatter(bl::formatter::high_precision_time("$mm.$ss:$micro ")); 
  tspikes_l()->writer().add_formatter(bl::formatter::append_newline()); 
  tspikes_l()->writer().add_destination( bl::destination::cout() );
  tspikes_l()->mark_as_initialized();

}


