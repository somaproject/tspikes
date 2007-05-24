#include "testspikes.h"
#include <iostream>
#include <fstream>


using namespace std; 

void testSpikes(vector<soma::Spike> * spikes) 
{
  

  // first one
  soma::SpikeWaveform swf; 
  swf.channel = 0; 
  swf.gain = 100;
  swf.DSPfilter = 10; 
  swf.hwfilter = 10; 
  swf.ADCrange = 4.096;
  swf.thold = 10000; 
  for (int i = 0; i < 32; i++)
    swf.waveform[i] = 0; 
  
  swf.waveform[8] = 16385; 
  
  soma::Spike spike; 
  spike.source = 0; 
  spike.chancount = 4; 
  spike.timestamp = 1000; 
  spike.tholdpos = 8; 
  spike.waveforms[0] = swf; 
  
  swf.channel=1; 
  swf.waveform[8] = 8384; 
  spike.waveforms[1] = swf; 
  swf.channel=2;
  spike.waveforms[2] = swf; 
  swf.channel = 3; 
  spike.waveforms[3] = swf; 

  spikes->push_back(spike); 
  
  spike.waveforms[0].waveform[8]=9000;
  spike.waveforms[1].waveform[8]=4000; 
  spike.waveforms[2].waveform[8]=3000; 
  spike.waveforms[3].waveform[8]=11000; 
  spike.timestamp = 2000; 
  
  spikes->push_back(spike);   

}

void loadSpikes(vector<soma::Spike> * spikes) {
  ifstream file("f212.tt", ios::binary); 
  
  char headbuf[200];
  headbuf[12] = 0; 
  string headstr = headbuf; 
  string endhead = "%%ENDHEADER";
  int filepos(0); 
  while(headstr != endhead) {
    file.getline(headbuf, 200); 
    headstr = headbuf; 
    
  }

  unsigned int chan; 
  unsigned int ts; 
  short wf[128];
  //file.read((char*)&chan, 4);
  // just get the timestaps, chris
  
  soma::SpikeWaveform swf; 
  swf.channel = 0; 
  swf.gain = 11991;
  swf.DSPfilter = 10; 
  swf.hwfilter = 10; 
  swf.ADCrange = 20;
  swf.thold = 320; 
  for (int i = 0; i < 32; i++)
    swf.waveform[i] = (0); 
  

  
  soma::Spike spike; 
  spike.source = 0; 
  spike.chancount = 4; 
  spike.timestamp = 1000; 
  spike.tholdpos = 8; 
  spike.waveforms[0] = swf; 
  spike.waveforms[1] = swf; 
  spike.waveforms[2] = swf; 
  spike.waveforms[3] = swf; 
 
  while(file.eof() == false)
    {
      file.read((char*)&ts, 4); 
      file.read((char*)wf, 256); 

      spike.timestamp = ts; 
      for (int j = 0; j < 32; j++) {
	
	spike.waveforms[0].waveform[j] = wf[j*4]*16;
	spike.waveforms[0].channel = 0 ;
	spike.waveforms[1].waveform[j] = wf[j*4+1]*16; 
	spike.waveforms[1].channel = 1; 
	spike.waveforms[2].waveform[j] = wf[j*4+2]*16;
	spike.waveforms[2].channel = 2;  
	spike.waveforms[3].waveform[j] = wf[j*4+3]*16;
	spike.waveforms[3].channel = 3;  
	
      }
      spikes->push_back(spike);
    }
}
