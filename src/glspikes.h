#ifndef GLSPIKES_H
#define GLSPIKES_H

#include <GL/gl.h>
#include <vector>
#include <list>

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
typedef boost::ptr_map<rtime_t, GLSPVect_t> GLSPVectpList_t; 

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


#endif //GLSPIKES_H
