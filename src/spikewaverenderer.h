#ifndef SPIKEWAVERENDERER_H
#define SPIKEWAVERENDERER_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <assert.h>
#include <boost/shared_ptr.hpp>

#include <stdlib.h>
#include <gtkmm.h>

#define GL_GLEXT_PROTOTYPES

#include <gtkglmm.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#include "glconfig.h"
#include "glspikes.h"

#include "cairogltext/glstring.h"
#include "somanetcodec.h" 

const int SPIKEWAVE_LEN = 32; 

typedef std::list<GLSpikeWave_t> SpikeWaveList_t; 

class SpikeWaveRenderer 
{
public:
  explicit SpikeWaveRenderer(GLChan_t chan); 

  virtual ~SpikeWaveRenderer();

  void setAmplitudeView(float min, float max); 
  void getAmplitudeView(float * min, float * max); 

  void setAmplitudeRange(float min,  float max); 
  void setViewPort(int viewportWinX, int viewportWinY); 
  
  void setTime(uint64_t time); 
  void newSpikeWave(const GLSpikeWave_t & sw); 
  void setListLen(int len);

  void setLive(bool); 
  
  void setState(TSpikeChannelState ts); 
  void setTriggerThreshold(float thold); 

  // Update window synchronously (fast).
  void setPaused(bool state); 
  
  void render(); 
  
  void setupOpenGL(); 

protected:

  void updateViewingWindow(); 
  void sanityCheckViewParameters(); 
  bool viewChanged_; 
  float decayVal_;  // how many ts are we at zero opacity with ? 
  GLChan_t chan_; 
  SpikeWaveList_t swl_; 
  bool spikeWaveListFull_; 
  int spikeWaveListTgtLen_; 
  uint64_t currentTime_; 
  GLfloat viewX1_, viewX2_, viewY1_, viewY2_; 
  GLint viewportWinX_, viewportWinY_; 

  GLfloat ampMin_, ampMax_; 
  GLfloat trigger_; 
  
  TSpikeChannelState channelState_; 
  bool renderSpikeWave(const GLSpikeWave_t & sw, 
		       float alpha, bool plotPoints);
  bool setViewingWindow(float x1, float y1,  float x2, float y2); 
  void renderGrid(); 
  void renderText(); 

  int m_Frames;
  GLString glString_; 
  GLuint texName1; 
  void setupTexture(); 
  void renderTexture(); 
  void renderRange(); 
  bool isLive_; 
  void renderPaused(); 
  void renderTrigger(); 


};

#endif //SPIKEWAVERENDERER_H
