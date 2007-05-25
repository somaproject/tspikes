#ifndef SPIKEWAVEVIEW_H
#define SPIKEWAVEVIEW_H

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

const int SPIKEWAVE_LEN = 32; 

typedef std::list<GLSpikeWave_t> SpikeWaveList_t; 

class SpikeWaveView : public Gtk::GL::DrawingArea
{
public:
  explicit SpikeWaveView(GLChan_t chan); 

  virtual ~SpikeWaveView();

  // Invalidate whole window.
  void inv(); 
  bool setViewingWindow(float x1, float y1,  float x2, float y2); 
  int getFrames(); 
  void setTime(uint64_t time); 
  void newSpikeWave(const GLSpikeWave_t & sw); 
  void setListLen(int len);



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

  bool renderSpikeWave(const GLSpikeWave_t & sw, 
		       float alpha, bool plotPoints);

  
  int m_Frames;

};

#endif //SPIKEWAVEVIEW_H
