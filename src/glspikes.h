#ifndef GLSPIKES_H
#define GLSPIKES_H

#include <GL/gl.h>
#include <vector>
#include <list>
#include <somanetwork/tspike.h>
#include <boost/utility.hpp>
#include <boost/ptr_container/ptr_map.hpp>
#include "somanetcodec.h"

// a relative time is some time, in seconds, since some event.
// Since openGL can't handle 64-bit floats, we need to use 32-bit floats, 
// but those only really have 24 bits of dynamic range. So if we kept
// track of time in seconds with a precision of ms, a float would
// lose precision after 12 days. 

enum GLChan_t {CHANX, CHANY, CHANA, CHANB}; 

struct GLSpikePoint_t {
  GLfloat p1; 
  GLfloat p2; 
  GLfloat p3; 
  GLfloat p4; 
  reltime_t rtime; 
  GLbyte tchan; 
}; 

typedef std::vector<GLSpikePoint_t> GLSPVect_t; 

// we use a map to give us sorted O(lg n ) access to the data 

typedef boost::ptr_map<reltime_t, GLSPVect_t> GLSPVectMap_t; 
inline GLSPVectMap_t::iterator getLastIter( GLSPVectMap_t & sp) {
  GLSPVectMap_t::iterator i; 
  i = sp.end(); 
  assert(sp.empty() == false); 
  
  --i; 
  return i; 
}

struct CRGBA_t 
{
  GLubyte R; 
  GLubyte G; 
  GLubyte B; 
  GLubyte A; 
} __attribute__((packed)) ; 

struct GLSpikeWave_t
{
  reltime_t rtime; 
  std::vector<GLfloat> wave; 
}; 

std::vector<GLSpikeWave_t> splitSpikeToGLSpikeWaves(const TSpike_t & tspike, abstime_t offset); 



reltime_t abstimeToRelTime(abstime_t t, abstime_t offset); 

abstime_t somatimeToAbsTime(somatime_t ts); 

reltime_t somatimeToRelTime(somatime_t ts, abstime_t offset); 

GLSpikePoint_t convertTSpikeToGLSpike(const TSpike_t & ts, abstime_t offset); 

void appendTSpikeToSPL(const GLSpikePoint_t & sp, 
		       GLSPVectMap_t * pspVectpList); 

#endif //GLSPIKES_H
