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
#include "ratetimelinerenderer.h"


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
  
  viewsignal_t & viewSignal(); 
  void setLive(bool);
  void setCursorTime(float time); 
  void setCursorVisible(bool visible); 
  void resetData(); 

protected:

  RateTimelineRenderer renderer_; 

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
  gdouble lastX_; 

};

#endif //RATETIMELINE_H
