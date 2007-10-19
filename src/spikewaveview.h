#ifndef SPIKEWAVEVIEW_H
#define SPIKEWAVEVIEW_H

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


const int SPIKEWAVE_LEN = 32; 

typedef std::list<GLSpikeWave_t> SpikeWaveList_t; 

class SpikeWaveView : public Gtk::GL::DrawingArea
{
public:
  explicit SpikeWaveView(GLChan_t chan); 

  virtual ~SpikeWaveView();

  // Invalidate whole window.
  void inv(); 

  void setAmplitudeView(float min, float max); 
  void setAmplitudeRange(float min,  float max); 
  bool setViewingWindow(float x1, float y1,  float x2, float y2); 

  int getFrames(); 
  void setTime(uint64_t time); 
  void newSpikeWave(const GLSpikeWave_t & sw); 
  void setListLen(int len);

  void setLive(bool); 

  // Update window synchronously (fast).
  void update()
  { get_window()->process_updates(false); }

  void setPaused(bool state); 

protected:

  // signal handlers:
  virtual void on_realize();
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_map_event(GdkEventAny* event);
  virtual bool on_unmap_event(GdkEventAny* event);
  virtual bool on_visibility_notify_event(GdkEventVisibility* event);
  virtual bool on_idle();

  void updateViewingWindow(); 
  bool viewChanged_; 
  float decayVal_;  // how many ts are we at zero opacity with ? 
  GLChan_t chan_; 
  SpikeWaveList_t swl_; 
  bool spikeWaveListFull_; 
  int spikeWaveListTgtLen_; 
  uint64_t currentTime_; 
  GLfloat viewX1_, viewX2_, viewY1_, viewY2_; 
  GLfloat ampMin_, ampMax_; 
  GLfloat trigger_; 

  bool renderSpikeWave(const GLSpikeWave_t & sw, 
		       float alpha, bool plotPoints);
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

  bool on_button_press_event(GdkEventButton* event); 
  bool on_motion_notify_event(GdkEventMotion* event); 


  float lastX_, lastY_; 
  float zeroPos_; 

};

#endif //SPIKEWAVEVIEW_H
