#include "glspikes.h"
#include <somanetwork/tspike.h>

std::vector<GLSpikeWave_t> splitSpikeToGLSpikeWaves(const TSpike_t & tspike)
{
  
  // Take in a tspike and spit out the spikes as spikewaves
  std::vector<GLSpikeWave_t> swvect(4); 
  swvect[0].ts = tspike.time; 
  swvect[1].ts = tspike.time; 
  swvect[2].ts = tspike.time; 
  swvect[3].ts = tspike.time; 

  float SCALE = 1e9; 
  
  for (int i = 0; i < TSPIKEWAVE_LEN; i++)
    {
      swvect[0].wave.push_back(float(tspike.x.wave[i]) / SCALE); 
      swvect[1].wave.push_back(float(tspike.y.wave[i]) / SCALE); 
      swvect[2].wave.push_back(float(tspike.a.wave[i]) / SCALE); 
      swvect[3].wave.push_back(float(tspike.b.wave[i]) / SCALE); 

    }

  return swvect; 
}


void appendTSpikeToSPL(const TSpike_t & tspike, GLSPVectpList_t * pspVectpList)
{
  GLSpikePoint_t sp; 
  sp.ts = tspike.time; 
  sp.p1 = -100e6; 
  sp.p2 = -100e6; 
  sp.p3 = -100e6; 
  sp.p4 = -100e6; 
  sp.tchan = - 1 ;
  
  // remember tspike is in nv
  float SCALE = 1e9; 

  for (int i = 0; i < TSPIKEWAVE_LEN; i++)
    {
      GLfloat xval = float(tspike.x.wave[i]) / SCALE; 
      GLfloat yval = float(tspike.y.wave[i]) / SCALE; 
      GLfloat aval = float(tspike.a.wave[i]) / SCALE; 
      GLfloat bval = float(tspike.b.wave[i]) / SCALE; 

      GLfloat xthresh = float(tspike.x.threshold) / SCALE; 
      GLfloat ythresh = float(tspike.y.threshold) / SCALE; 
      GLfloat athresh = float(tspike.a.threshold) / SCALE; 
      GLfloat bthresh = float(tspike.b.threshold) / SCALE; 

      if (sp.p1 < xval)
	sp.p1 = xval; 
      
      if (sp.tchan < 0 & xval > xthresh)
	{
	  sp.tchan = 0; 
	}

      if (sp.p2 < yval)
	sp.p2 = yval; 
      
      if (sp.tchan < 0 & yval > ythresh)
	{
	  sp.tchan = 1; 
	}

      if (sp.p3 < aval)
	sp.p3 = aval; 
      
      if (sp.tchan < 0 & aval > athresh)
	{
	  sp.tchan = 2; 
	}

      if (sp.p4 < bval)
	sp.p4 = bval; 
      
      if (sp.tchan < 0 & bval > bthresh)
	{
	  sp.tchan = 3; 
	}
      
    }
  

  (--pspVectpList->end())->second->push_back(sp); 
 
}
