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

typedef float rateval_t; 
typedef float timeval_t; 

struct RatePoint_t
{
  timeval_t time; 
  rateval_t rate; 

};

class RateTimeline : public Gtk::GL::DrawingArea
{
public:
  explicit RateTimeline(); 

  virtual ~RateTimeline();

  void appendRate(RatePoint_t rp); 

  // Update window synchronously (fast).
  void update()
  { get_window()->process_updates(false); }
  
  void invalidate() {
    Glib::RefPtr<Gdk::Window> win = get_window();
    Gdk::Rectangle r(0, 0, get_allocation().get_width(),
		     get_allocation().get_height());
    win->invalidate_rect(r, false);

  }

  typedef sigc::signal<void, bool, float, float> viewsignal_t;
  viewsignal_t viewSignal(); 

  void setLive(bool);
  void setCursorTime(float time); 
  void setCursorVisible(bool visible); 


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

  bool on_enter_notify_event(GdkEventCrossing * event); 
  bool on_leave_notify_event(GdkEventCrossing * event); 

  void updateViewingWindow();
  
  // primary data source
  const float BASEDURATION; 
  // state variables
  int majorTick_; 
  int minorTick_; 
  
  float viewT1_, viewT2_, viewX1_, viewX2_; 
  float lastX_; 
  std::vector<RatePoint_t> rates_; 
  
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
  void renderCursor(); 

};

#endif //RATETIMELINE_H
