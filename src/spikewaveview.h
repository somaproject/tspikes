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
#include "spikewaverenderer.h" 
#include "somanetcodec.h"

class SpikeWaveView : public Gtk::GL::DrawingArea
{
public:
  explicit SpikeWaveView(GLChan_t chan); 

  virtual ~SpikeWaveView();

  void setAmplitudeView(float min, float max); 
  
  int getFrameCount(); 

  void setTime(uint64_t time); 
  void newSpikeWave(const GLSpikeWave_t & sw); 
  void updateState(const TSpikeChannelState & state); 
  
  void setListLen(int len);
  void setLive(bool); 

  // Invalidate whole window.
  void invalidate(); 

protected:

  SpikeWaveRenderer spikeWaveRenderer_; 


  // signal handlers:
  virtual void on_realize();
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_map_event(GdkEventAny* event);
  virtual bool on_unmap_event(GdkEventAny* event);
  virtual bool on_visibility_notify_event(GdkEventVisibility* event);
  virtual bool on_idle();

  void updateViewingWindow(); 

  uint64_t currentTime_; 

  int frameCount_;
  bool on_button_press_event(GdkEventButton* event); 
  bool on_motion_notify_event(GdkEventMotion* event); 
  
  float lastX_, lastY_; 
  float zeroPos_; 

  // Update window synchronously (fast).
  void update()
  { get_window()->process_updates(false); }
  
  bool live_; 


};

#endif //SPIKEWAVEVIEW_H
