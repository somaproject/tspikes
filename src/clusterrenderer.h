#ifndef CLUSTERRENDERER_H
#define CLUSTERRENDERER_H 

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <assert.h>

#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "glconfig.h"
#include "glspikes.h"
#include "cairogltext/glstring.h"


enum DecayMode {LINEAR, LOG}; 
enum CViewMode  {VIEW12, VIEW13, VIEW14, VIEW23, VIEW24, VIEW34}; 


class ClusterRenderer 
{
 public:
  explicit ClusterRenderer(GLSPVectpList_t * pspvl, CViewMode cvm);  
  virtual ~ClusterRenderer();
  void setView(GLSPVectpList_t::iterator sstart, 
		   GLSPVectpList_t::iterator send, 
		   float decayRate, DecayMode dm); 
  void setGrid(float); 

  bool setViewingWindow(float x1, float y1,  float x2, float y2); 
  
  void setup(); 
  void render(); 

  void reset(); 

  void resetAccumBuffer(GLSPVectpList_t::iterator sstart, 
			GLSPVectpList_t::iterator send); 
  void renderSpikeVector(const GLSPVect_t & spvect, 
			 bool live = false); 
  void updateView(); 
  void updateViewingWindow(); 
  
 protected:
  
  bool isSetup_; 
  GLSPVectpList_t*  pspvl_; 
  float decayRate_; 
  DecayMode decayMode_; 
  bool viewChanged_;   
  GLSPVectpList_t::iterator viewStartIter_;  
  GLSPVectpList_t::iterator viewEndIter_;  

  CViewMode viewMode_; 
  GLSPVectpList_t::iterator pCurSPVect_; 
  GLfloat viewX1_, viewX2_, viewY1_, viewY2_; 
  
  GLuint gpuProg_; 
  GLString glString_; 

  float gridSpacing_; 
  void renderGrid(); 
  
  std::string Xlabel_, Ylabel_; 
};
#endif
