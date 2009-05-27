#include "glspikes.h"
#include <somanetwork/tspike.h>

std::vector<GLSpikeWave_t> splitSpikeToGLSpikeWaves(const TSpike_t & tspike, 
						    abstime_t offset)
{
  
  // Take in a tspike and spit out the spikes as spikewaves
  std::vector<GLSpikeWave_t> swvect(4); 
  reltime_t rt = somatimeToRelTime(tspike.time, offset); 
  swvect[0].rtime = rt; 
  swvect[1].rtime = rt; 
  swvect[2].rtime = rt; 
  swvect[3].rtime = rt; 

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

abstime_t somatimeToAbsTime(somatime_t ts) {

  return abstime_t(ts) / 50e3; 
}

reltime_t abstimeToRelTime(abstime_t t, abstime_t offset)
{
  return t - offset; 

}
reltime_t somatimeToRelTime(somatime_t ts, abstime_t offset)
{
  // convert 
  abstime_t dts = somatimeToAbsTime(ts); 
  return abstimeToRelTime(dts, offset); 

}



GLSpikePoint_t convertTSpikeToGLSpike(const TSpike_t & ts, abstime_t offset)
{
  // offset is used for extracting the relative time

  GLSpikePoint_t sp; 

  sp.rtime = somatimeToRelTime(ts.time, offset); 
  sp.p1 = std::numeric_limits<float>::min(); 
  sp.p2 = std::numeric_limits<float>::min(); 
  sp.p3 = std::numeric_limits<float>::min(); 
  sp.p4 = std::numeric_limits<float>::min(); 
  sp.tchan = - 1 ;
  
  // remember tspike is in nv
  float SCALE = 1e9; 

  for (int i = 0; i < TSPIKEWAVE_LEN; i++)
    {
      GLfloat xval = float(ts.x.wave[i]) / SCALE; 
      GLfloat yval = float(ts.y.wave[i]) / SCALE; 
      GLfloat aval = float(ts.a.wave[i]) / SCALE; 
      GLfloat bval = float(ts.b.wave[i]) / SCALE; 

      GLfloat xthresh = float(ts.x.threshold) / SCALE; 
      GLfloat ythresh = float(ts.y.threshold) / SCALE; 
      GLfloat athresh = float(ts.a.threshold) / SCALE; 
      GLfloat bthresh = float(ts.b.threshold) / SCALE; 

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
  
  return sp; 
}

void appendTSpikeToSPL(const GLSpikePoint_t & sp, 
		       GLSPVectMap_t * pspVectpList)
{


  (--pspVectpList->end())->second->push_back(sp); 
 
}
