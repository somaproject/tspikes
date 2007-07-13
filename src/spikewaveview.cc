#include "spikewaveview.h"
#include "shaders.h"
#include "glconfig.h"
#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <sys/time.h>
#include <time.h>


int SpikeWaveView::getFrames()
{
  int f = m_Frames; 
  m_Frames = 0; 
  return f; 

}

SpikeWaveView::SpikeWaveView(GLChan_t chan) : 
  chan_(chan), 
  viewChanged_(false),
  decayVal_(0.0), 
  spikeWaveListFull_(false), 
  spikeWaveListTgtLen_(25), 
  m_Frames(0),
  isLive_(true)
{

  //
  // Configure OpenGL-capable visual.
  //

  Glib::RefPtr<Gdk::GL::Config> glconfig;
  
  // Try double-buffered visual
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGBA   |
                                     Gdk::GL::MODE_DOUBLE | 
				     Gdk::GL::MODE_ACCUM);
  if (!glconfig)
    {
      throw std::runtime_error("Could not acquire double-buffered visual"); 
    }

  // print frame buffer attributes.
  GLConfigUtil::examine_gl_attrib(glconfig);

  set_gl_capability(glconfig);

  // Add events.
  add_events(Gdk::VISIBILITY_NOTIFY_MASK);


}

SpikeWaveView::~SpikeWaveView()
{
}

void SpikeWaveView::newSpikeWave(const GLSpikeWave_t & sw)
{
  //std::cout << sw.wave[0] << std::endl; 
  if (spikeWaveListFull_)
    {
      swl_.push_back(sw); 
      swl_.pop_front(); 
    } 
  else 
    {
      swl_.push_back(sw); 
      if (swl_.size() >= spikeWaveListTgtLen_)
	{
	  spikeWaveListFull_ = true; 

	}
      
    }
  
  
}

void SpikeWaveView::setListLen(int len)
{
  
  spikeWaveListTgtLen_ = len; 
  spikeWaveListFull_ = false; 
  swl_.clear(); 
  
}

void SpikeWaveView::on_realize()
{

  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return;

  //glEnable(GL_NORMALIZE);
  glEnable(GL_BLEND); 
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	


  glEnable(GL_LINE_SMOOTH); 
  glEnable(GL_POINT_SMOOTH); 
  glEnableClientState(GL_VERTEX_ARRAY); 
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  updateViewingWindow(); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***

}

bool SpikeWaveView::setViewingWindow(float x1, float y1, 
				   float x2, float y2)
{
  viewX1_ = x1; 
  viewY1_ = y1; 
  viewX2_ = x2; 
  viewY2_ = y2; 

  viewChanged_ = true; 
}

void SpikeWaveView::updateViewingWindow()

{
  
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 

  glOrtho(viewX1_, viewX2_, viewY1_, viewY2_, -3, 3); 

  int winX, winY; 
  winX = get_width(); 
  winY = get_height(); 
  glViewport(0, 0, winX, winY); 


  viewChanged_ = false; 
  
}

bool SpikeWaveView::on_configure_event(GdkEventConfigure* event)
{

  std::cout << "SPikeWaveView::on_configure_event" << std::endl;

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 
  glDrawBuffer(GL_BACK); 
  
  viewChanged_ = true; 

//   glClearColor(0.0, 0.0, 0.0, 1.0); 
//   glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***

  //invalidate(); 
  return true;
}

bool SpikeWaveView::renderSpikeWave(const GLSpikeWave_t & sw, 
				    float alpha, 
				    bool plotPoints = false)
{
  // assume we are in a primary GL loop
  // first we render the line  
  switch (chan_) 
    {
    case CHANX:
      glColor4f(1.0, 0.0, 0.0, alpha); 
      break;

    case CHANY:
      glColor4f(0.0, 1.0, 0.0, alpha); 
      break;

    case CHANA:
      glColor4f(0.0, 0.0, 1.0, alpha); 
      break;

    case CHANB:
      glColor4f(1.0, 1.0, 0.0, alpha); 
      break;

    default:
      break;
    }


  if (plotPoints)
    {
      glLineWidth(3.0); 
    } 
  else
    {
      glLineWidth(1.0); 
    }
  
  glBegin(GL_LINE_STRIP); 
  for (unsigned int i = 0; i < sw.wave.size(); i++)
    {
      glVertex2f(i, sw.wave[i]); 
    }
  glEnd(); 

  // now we optionally render the points
  if (plotPoints)
    {
      glColor4f(1.0, 1.0, 1.0, alpha); 
      glPointSize(3.0); 
      glBegin(GL_POINTS); 
	for (unsigned int i = 0; i < sw.wave.size(); i++)
	  {
	    glVertex2f(i, sw.wave[i]); 
	  }
      glEnd(); 
    }
  return true; 

}

bool SpikeWaveView::on_expose_event(GdkEventExpose* event)
{


  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  if (viewChanged_)
    {
      updateViewingWindow(); 
    }
  
  if (isLive_) { 
    glClearColor(0.0, 0.0, 0.0, 1.0); 
    glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
    
    renderGrid(); 
    
    // real work 
    SpikeWaveList_t::iterator csIter; 
    // always show the last N spikes, and only the last N. 
    float alpha = 0.5; 
    
    for (csIter = swl_.begin(); 
	 csIter != swl_.end(); 
	 csIter ++ ) 
    {
      alpha -= decayVal_; 
      if (alpha < 0.0)
	{
	  alpha = 0.0; 
	}
      renderSpikeWave(*csIter, alpha, false); 
    }
    
    if (! swl_.empty()) 
    {
      renderSpikeWave(swl_.back(), 1.0, true); 
    }
    
    int y = get_height(); 
    
    glString_.drawWinText(4, y-15, "Gain : 100", 10); 
    glString_.drawWinText(4, y-25, "HW HPF: On ", 10); 
    glString_.drawWinText(4, y-35, "Filter : 6 kHz ", 10); 
    glString_.drawWinText(4, y-45, "Thold : 320 uV ", 10); 
    
  } else { 
    // we are in a "pasued state" 
    // don't show new data

    renderPaused();

  }
    
  // Swap buffers.
  gldrawable->swap_buffers();
  gldrawable->gl_end();

  ++m_Frames;
  
  return true;
}

bool SpikeWaveView::on_map_event(GdkEventAny* event)
{

  std::cout << "SPikeWaveView::on_map_event" << std::endl;
  
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 

  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  
  updateViewingWindow(); 

  gldrawable->wait_gl(); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***


  return true;
}

bool SpikeWaveView::on_unmap_event(GdkEventAny* event)
{

  return true;
}

bool SpikeWaveView::on_visibility_notify_event(GdkEventVisibility* event)
{

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 
  glDrawBuffer(GL_BACK); 

  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();

  return true;
}

bool SpikeWaveView::on_idle()
{
  
  return true;
}


void SpikeWaveView::inv() {
  Glib::RefPtr<Gdk::Window> win = get_window();
  Gdk::Rectangle r(0, 0, get_allocation().get_width(),
		   get_allocation().get_height());
  win->invalidate_rect(r, false);

}

void SpikeWaveView::setTime(uint64_t ts)
{
  currentTime_ = ts; 
}

void SpikeWaveView::renderGrid()
{
  // draw the various vertical markings, at 100 uV
  const float GRIDINC = 100e-6; 

  glLineWidth(3.0); 
   
  glColor4f(1.0, 1.0, 1.0, 1.0); 
  // plot zero line
  glBegin(GL_LINES); 
  glVertex2f(viewX1_, 0.0); 
  glVertex2f(viewX2_, 0.0); 
  glEnd(); 

  glColor4f(1.0, 1.0, 1.0, 0.5); 
  glLineWidth(1.0); 
 
  float ydelta = viewY2_ - viewY1_; 
  int lnumpos = int(viewY2_/GRIDINC); 

  for (int i = 0; i < lnumpos; i++)
    {
      glBegin(GL_LINES); 
      glVertex2f(viewX1_, GRIDINC*(i+1)); 
      glVertex2f(viewX2_, GRIDINC*(i+1)); 
      glEnd(); 
    }
  
}


void SpikeWaveView::setPaused(bool state)
{
  

}

void SpikeWaveView::renderPaused()
{

  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 
  
  int MAXX = get_width(); 
  int MAXY = get_height(); 
  glOrtho(0, MAXX,
	  0, MAXY, -3, 3); 
  
  timeval t; 
  gettimeofday(&t, NULL); 
  float intens = float(t.tv_sec) * 1000000  + float(t.tv_usec); 
  float alpha = sin(intens/1e6 / 5); 

  
  glColor4f(0.7, 0.0, 0.0, 0.3); 
  
  glBegin(GL_POLYGON);
  glVertex2i(0, MAXY/2 + 20); 
  glVertex2i(MAXX, MAXY/2 + 20); 
  glVertex2i(MAXX, MAXY/2 - 20);
  glVertex2i(0, MAXY/2 -20); 
  glEnd(); 

  glColor4f(1.0, 1.0, 1.0, 1.0); 
  glString_.drawWinText(0, MAXY/2, "PAUSED", 30); 


}

void SpikeWaveView::setLive(bool l)
{
  isLive_ = l; 
  viewChanged_ = true; 
  inv(); 


}
