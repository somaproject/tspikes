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

protected:

  // signal handlers:
  virtual void on_realize();
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_map_event(GdkEventAny* event);
  virtual bool on_unmap_event(GdkEventAny* event);
  virtual bool on_visibility_notify_event(GdkEventVisibility* event);

  void updateViewingWindow();
  
  // primary data source
  
  // state variables
  int majorTick_; 
  int minorTick_; 
  
  int decayRange_; 
  int cutoffPos_; 
  bool viewLatest_; 
  float viewX1_, viewX2_, viewY1_, viewY2_; 
  std::vector<RateVal_t> rates_; 
  
  GLuint gpuProgGradient_; 

};

#endif //RATETIMELINE_H
