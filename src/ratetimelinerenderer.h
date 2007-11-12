#ifndef RATETIMELINERENDERER_H
#define RATETIMELINERENDERER_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <assert.h>

#include <stdlib.h>
#include <gtkmm.h>

#define GL_GLEXT_PROTOTYPES

#include <gtkglmm.h>

#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <boost/shared_ptr.hpp>

#include "glspikes.h"
#include "cairogltext/glstring.h"


typedef float rateval_t; 
typedef float timeval_t; 

struct RatePoint_t
{
  timeval_t time; 
  rateval_t rate; 

};

typedef std::vector<RatePoint_t> ratevector_t; 

typedef sigc::signal<void, bool, float, float> viewsignal_t;


class RateTimelineRenderer
{
public:
  RateTimelineRenderer(); 
  ~RateTimelineRenderer(); 

  bool appendRate(RatePoint_t rp); 

  // Update window synchronously (fast).

  void setLive(bool);
  bool getLive(); 
  void setCursorTime(float time); 
  float getCursorTime(); 

  void setCursorVisible(bool visible); 
  void setup(); 
  void render(); 

  float getViewT1(); 
  float getViewT2(); 
  void setViewT(float, float); 
  void setActivePos(float); 
  timeval_t getStartTime(); 
  bool ratesEmpty(); 
  viewsignal_t & viewSignal(); 
  void setViewPort(int viewportWinX, int viewportWinY); 

protected:

  // signal handlers:
  void updateViewingWindow();
  
  // primary data source
  const float BASEDURATION; 
  // state variables
  int majorTick_; 
  int minorTick_; 
  
  float viewT1_, viewT2_, viewX1_, viewX2_; 
  float lastX_; 
  ratevector_t rates_; 
  int viewportWinX_, viewportWinY_; 
  
  GLuint gpuProgGradient_; 

  viewsignal_t viewSignal_; 

  // selection/active range view variables
  bool isLive_; 
  float decayRate_; 
  float activePos_; 

  void drawTicks(); 

  // time cursor
  float cursorTime_; 
  bool cursorVisible_; 
  float cursorOpacity_; 
  GLString cursorString_; 

  void renderCursor(); 
  void renderStartCursor(); 
  void renderLatestCursor(); 

};

#endif //RATETIMELINE_H
