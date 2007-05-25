#include "ttreader.h"
#include <iostream>
#include "network/data/tspike.h"
#include <boost/regex.hpp>
#include <sstream>

using namespace std; 

template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}

ttreader::ttreader(std::string filename) 
{
  file_.open(filename.c_str(), std::ios::in | std::ios::binary);
  // position the file appropriately

  std::string line; 
  while (line != "%%ENDHEADER") {
    getline(file_, line); 
    
//    boost::regex expr("% channel ([0-9]) (\\w+):\\s+([0-9]+).*");
//    boost::cmatch matches ;
    
//    if(boost::regex_match(line.c_str(), matches, expr) )
//      {
//        std::
//        std::string schan(matches[1].first, matches[1].second); 
//        std::string stype(matches[2].first, matches[2].second); 
//        std::string sval(matches[3].first, matches[3].second); 
    
//      }
    
    
    istringstream sin(line); 
    char per; 
    std::string chantext, keytext; 
    int chanint, chanval; 
    
    sin >> per; 
    sin >> chantext; 
    sin >> chanint; 
    
    if(per == '%' and chantext == "channel") 
      {
	sin >> keytext; 
	sin >> chanval; 
	
	if (keytext == "ampgain:") {
	  gains_[chanint] = chanval; 
	}
	
	if (keytext == "threshold:") {
	  thresholds_[chanint] = chanval; 
	}
	
      }
    
  }
  // print out the results
  for (int i = 0; i < 8; i++) {
    std::cout << "For channel " << i << " gain = " << gains_[i] 
	      << " thold = " << thresholds_[i] << std::endl; 
  }
}

int32_t to_nv(int16_t val, int gain)
{
  int32_t x; 
  x = int ((float(val)) / 4096 * 10.0 / gain  * 1e9); 

  return x; 
}

TSpike_t ttreader::getTSpike()
{
  TSpike_t tspike; 
  uint32_t time; 
  file_.read((char*)&(time), sizeof(time));   
  tspike.time = time; 
  
  int   coffset = 0; 
  
  tspike.x.filtid = 0; 
  tspike.y.filtid = 1; 
  tspike.a.filtid = 2; 
  tspike.b.filtid = 3; 
  
  tspike.x.valid = 1; 
  tspike.y.valid = 1; 
  tspike.a.valid = 1; 
  tspike.b.valid = 1; 
  
  tspike.x.threshold = to_nv(thresholds_[coffset + 0], 
			     gains_[coffset + 0]); 

  tspike.y.threshold = to_nv(thresholds_[coffset + 1], 
			     gains_[coffset + 1]); 

  tspike.a.threshold = to_nv(thresholds_[coffset + 2], 
			     gains_[coffset + 2]); 

  tspike.b.threshold = to_nv(thresholds_[coffset + 3], 
			     gains_[coffset + 3]); 


  for (int i = 0; i < 32; i++){ 
    uint16_t x; 
    file_.read((char *)&(x), sizeof(x)); 
    tspike.x.wave[i] = to_nv(x, gains_[coffset+0]); 
    
    uint16_t y; 
    file_.read((char *)&(y), sizeof(y)); 
    tspike.y.wave[i] = to_nv(y, gains_[coffset+1]); 
    
    uint16_t a; 
    file_.read((char *)&(a), sizeof(a)); 
    tspike.a.wave[i] = to_nv(a, gains_[coffset+2]); 
    
    uint16_t b; 
    file_.read((char *)&(b), sizeof(b)); 
    tspike.b.wave[i] = to_nv(b, gains_[coffset+3]); 
    
  }
  return tspike; 
  
}

int ttreader::getSpike(ttspike * sp){
  file_.read((char*)&(sp->ts), sizeof(sp->ts));

  for (int i = 0; i < 32; i++){ 
    file_.read((char *)&(sp->w1[i]), sizeof(sp->w1[0])); 
    file_.read((char *)&(sp->w2[i]), sizeof(sp->w1[0])); 
    file_.read((char *)&(sp->w3[i]), sizeof(sp->w1[0])); 
    file_.read((char *)&(sp->w4[i]), sizeof(sp->w1[0])); 
  }

}

bool ttreader::eof() {
  return file_.eof() ;
}

// void ttreader::getPeaks(ttspike *spike, SpikePoint *sp) 
// {
//   sp->ts = spike->ts; 
//   sp->p1 = std::numeric_limits<int>::min(); 
//   sp->p2 = std::numeric_limits<int>::min(); 
//   sp->p3 = std::numeric_limits<int>::min(); 
//   sp->p4 = std::numeric_limits<int>::min(); 
  

//   for (int i = 0; i < 32; i++){
//     int v1 = int(round(spike->w1[i] * 10.0 / 32768 * 1e9 / gain_ )); 
//     int v2 = int(round(spike->w2[i] * 10.0 / 32768 * 1e9 / gain_ )); 
//     int v3 = int(round(spike->w3[i] * 10.0 / 32768 * 1e9 / gain_ )); 
//     int v4 = int(round(spike->w4[i] * 10.0 / 32768 * 1e9 / gain_ )); 
//     if (sp->p1 < v1)
//       sp->p1 = v1;
//     if (sp->p2 < v2)
//       sp->p2 = v2;
//     if (sp->p3 < v3)
//       sp->p3 = v3;
//     if (sp->p4 < v4)
//       sp->p4 = v4;
//   }
//   int peakval =  std::numeric_limits<int>::min(); 
  
//   if (sp->p1 > peakval) {
//     peakval = sp->p1; 
//     sp->trigChan = 1; 
//   }

//   if (sp->p2 > peakval) {
//     peakval = sp->p2; 
//     sp->trigChan = 2; 
//   }

//   if (sp->p3 > peakval) {
//     peakval = sp->p3; 
//     sp->trigChan = 3; 
//   }

//   if (sp->p4 > peakval) {
//     peakval = sp->p4; 
//     sp->trigChan = 4; 
//   }

// }
