#include "ttreader.h"
#include <iostream>

using namespace std; 

ttreader::ttreader(std::string filename) {
 file_.open(filename.c_str(), std::ios::in | std::ios::binary);
 // position the file appropriately

 std::string line; 
 while (line != "%%ENDHEADER") {
   getline(file_, line); 
 }

 gain_ =1000; 
 
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
