#include "spikewaveview.h"
#include "shaders.h"
#include "glconfig.h"
#include <cairomm/context.h>
#include <cairomm/surface.h>

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
  m_Frames(0)
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

  //
  // Set OpenGL-capability to the widget.
  //

  set_gl_capability(glconfig);

  // Add events.
  add_events(Gdk::VISIBILITY_NOTIFY_MASK);


}

SpikeWaveView::~SpikeWaveView()
{
}

void SpikeWaveView::newSpikeWave(const GLSpikeWave_t & sw)
{
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
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
  glEnable(GL_BLEND); 

  glEnable(GL_LINE_SMOOTH); 
  glEnable(GL_POINT_SMOOTH); 
  glEnableClientState(GL_VERTEX_ARRAY); 
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  updateViewingWindow(); 
  pGLString_ = boost::shared_ptr<GLString>(new GLString("This is Test", 40)); 
  
  
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
  glLoadIdentity(); 

  glOrtho(viewX1_, viewX2_, viewY1_, viewY2_, -3, 3); 

  glViewport(0, 0, get_width(), get_height());
  viewChanged_ = false; 
  
}

bool SpikeWaveView::on_configure_event(GdkEventConfigure* event)
{


  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 
  glDrawBuffer(GL_BACK); 

  updateViewingWindow(); 

  glClearColor(0.0, 0.0, 0.0, 1.0); 
  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  

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

  pGLString_->render(); 

  // Swap buffers.
  gldrawable->swap_buffers();
  gldrawable->gl_end();

  ++m_Frames;
  
  return true;
}

bool SpikeWaveView::on_map_event(GdkEventAny* event)
{

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 

  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  
  updateViewingWindow(); 

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
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

  // Invalidate the whole window.
  
  return true;
}


void SpikeWaveView::inv() {
  get_window()->invalidate_rect(get_allocation(), true);
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


