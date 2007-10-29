#ifndef GLSPIKES_H
#define GLSPIKES_H

#include <GL/gl.h>
#include <vector>
#include <list>
#include <somanetwork/tspike.h>

#include <boost/ptr_container/ptr_map.hpp>

typedef double rtime_t ; 

enum GLChan_t {CHANX, CHANY, CHANA, CHANB}; 


struct GLSpikePoint_t {
  GLfloat p1; 
  GLfloat p2; 
  GLfloat p3; 
  GLfloat p4; 
  GLfloat ts; 
  GLbyte tchan; 
}; 

typedef std::vector<GLSpikePoint_t> GLSPVect_t; 

// we use a map to give us sorted O(lg n ) access to the data 

typedef boost::ptr_map<rtime_t, GLSPVect_t> GLSPVectpList_t; 
inline GLSPVectpList_t::iterator getLastIter( GLSPVectpList_t & sp) {
  GLSPVectpList_t::iterator i; 
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
  uint64_t ts; 
  std::vector<GLfloat> wave; 
}; 

std::vector<GLSpikeWave_t> splitSpikeToGLSpikeWaves(const TSpike_t & tspike); 
void appendTSpikeToSPL(const TSpike_t & tspike, GLSPVectpList_t * pspVectpList); 



#endif //GLSPIKES_H
