#ifndef CLUSTERRENDERER_H
#define CLUSTERRENDERER_H 

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <assert.h>

#include <stdlib.h>
#include <boost/format.hpp>

#define GL_GLEXT_PROTOTYPES

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "glconfig.h"
#include "glspikes.h"
#include "cairogltext/glstring.h"
#include "spvectdb.h"

enum DecayMode {LINEAR, LOG}; 
enum CViewMode  {VIEW12, VIEW13, VIEW14, VIEW23, VIEW24, VIEW34}; 

std::pair<int, int>  getCViewPair(CViewMode); 

class ClusterRenderer 
{
 public:
  explicit ClusterRenderer(SpikePointVectDatabase & pspvl, CViewMode cvm);  
  virtual ~ClusterRenderer();
  void setView(GLSPVectMap_t::const_iterator sstart, 
		   GLSPVectMap_t::const_iterator send, 
		   float decayRate, DecayMode dm); 
  void setGrid(float); 

  bool setViewingWindow(float x1, float y1,  float x2, float y2); 
  
  void setup(); 
  void render(); 

  void resetData(); 
  void reset(); 

  void resetAccumBuffer(GLSPVectMap_t::const_iterator sstart, 
			GLSPVectMap_t::const_iterator send); 
  void renderSpikeVector(const GLSPVect_t & spvect, 
			 bool live = false); 
  void renderSpikeVector(GLSPVectMap_t::const_iterator i, 
			 bool live = false); 

  void updateView(); 
  void updateViewingWindow(); 

  void setRangeBoxVisible(bool); 

  void fadeInText(); 
  void fadeOutText(); 

  void setRange(float x, float y) {
    rangeX_ = x; 
    rangeY_ = y; 
  }
  
 protected:
  
  bool isSetup_; 
  SpikePointVectDatabase  &   spvdb_; 
  float decayRate_; 
  DecayMode decayMode_; 
  bool viewChanged_;   
  GLSPVectMap_t::const_iterator viewStartIter_;  
  GLSPVectMap_t::const_iterator viewEndIter_;  

  CViewMode viewMode_; 
  GLSPVectMap_t::const_iterator pCurSPVect_; 
  GLfloat viewX1_, viewX2_, viewY1_, viewY2_; 
  
  GLuint gpuProg_; 
  GLString glString_; 
  GLString glHScaleString_; 
  GLString glVScaleString_; 

  float gridSpacing_; 
  void renderGrid(); 
  void renderHGrid(); 
  void renderVGrid(); 
  std::string Xlabel_, Ylabel_; 

  bool rangeBoxVisible_; 
  float rangeX_, rangeY_; 
  void drawRangeBox(); 
  
  sigc::connection textFadeConn_; 
  float textAlpha_; 
  bool resetPending_; 
  bool fadeInTextHandler(); 
  bool fadeOutTextHandler(); 

  std::vector<float> axisLabels_; 
};




#endif
