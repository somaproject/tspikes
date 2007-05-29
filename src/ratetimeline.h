#ifndef RATETIMELINE_H
#define RATETIMELINE_H

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

#include "glconfig.h"
#include "glspikes.h"

typedef float RateVal_t; 

class RateTimeline : public Gtk::GL::DrawingArea
{
public:
  explicit RateTimeline(); 

  virtual ~RateTimeline();

  void appendRate(RateVal_t x); 


  // Invalidate whole window.
  void inv(); 

  // Update window synchronously (fast).
  void update()
  { get_window()->process_updates(false); }
  
  typedef sigc::signal<void, bool, float, float> viewsignal_t;
  viewsignal_t viewSignal(); 

  
protected:

  // signal handlers:
  virtual void on_realize();
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_map_event(GdkEventAny* event);
  virtual bool on_unmap_event(GdkEventAny* event);
  virtual bool on_visibility_notify_event(GdkEventVisibility* event);

  // mouse gui 
  bool on_button_press_event(GdkEventButton* event);
  bool on_motion_notify_event(GdkEventMotion* event);
  bool on_scroll_event(GdkEventScroll* event);
  
  void updateViewingWindow();
  void setZoom(float zoomval, float tcenter);
  
  // primary data source
  const float BASEDURATION; 
  // state variables
  int majorTick_; 
  int minorTick_; 
  
  float viewT1_, viewT2_, viewX1_, viewX2_; 
  float lastX_; 
  float zoomLevel_; 
  std::vector<RateVal_t> rates_; 
  
  GLuint gpuProgGradient_; 

  viewsignal_t viewSignal_; 

  // selection/active range view variables
  bool isLive_; 
  float decayRate_; 
  float activePos_; 


};

#endif //RATETIMELINE_H
