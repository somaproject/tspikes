#ifndef GLSPIKES_H
#define GLSPIKES_H

#include <GL/gl.h>
#include <vector>
#include <list>

struct GLSpikePoint {
  GLfloat p1; 
  GLfloat p2; 
  GLfloat p3; 
  GLfloat p4; 
  GLfloat ts; 
  GLbyte tchan; 
}; 

typedef std::vector<GLSpikePoint> GLSPVect; 
typedef std::list<GLSPVect* > GLSPVectpList; 

struct CRGBA 
{
  GLubyte R; 
  GLubyte G; 
  GLubyte B; 
  GLubyte A; 
} __attribute__((packed)) ; 


#endif //GLSPIKES_H
