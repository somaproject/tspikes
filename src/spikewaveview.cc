#include "spikewaveview.h"
#include "shaders.h"
#include "glconfig.h"
#include <cairomm/context.h>
#include <cairomm/surface.h>
#include <sys/time.h>
#include <time.h>


SpikeWaveView::SpikeWaveView(GLChan_t chan) : 
  spikeWaveRenderer_(chan), 
  frameCount_(0), 
  live_(false), 
  constructed_(false)
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
  add_events(Gdk::VISIBILITY_NOTIFY_MASK | 
	     Gdk::BUTTON1_MOTION_MASK    | 
	     Gdk::BUTTON2_MOTION_MASK    | 
	     Gdk::BUTTON_PRESS_MASK );

  signal_motion_notify_event().connect(sigc::mem_fun(*this, 
						     &SpikeWaveView::on_motion_notify_event)); 

  signal_button_press_event().connect(sigc::mem_fun(*this, 
						    &SpikeWaveView::on_button_press_event)); 


}

SpikeWaveView::~SpikeWaveView()
{
}

void SpikeWaveView::newSpikeWave(const GLSpikeWave_t & sw)
{
  spikeWaveRenderer_.newSpikeWave(sw); 
  invalidate(); 
}

void SpikeWaveView::setListLen(int len)
{
  
  spikeWaveRenderer_.setListLen(len); 
  
}

void SpikeWaveView::on_realize()
{

  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return;

  spikeWaveRenderer_.setupOpenGL(); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***

}

void SpikeWaveView::setAmplitudeView(float min, float max)
{
  spikeWaveRenderer_.setAmplitudeView(min, max); 
  if (!constructed_) {
    constructed_ = true; 
  } else {
    invalidate();
  }
}

bool SpikeWaveView::on_configure_event(GdkEventConfigure* event)
{

  //std::cout << "SPikeWaveView::on_configure_event" << std::endl;

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  spikeWaveRenderer_.setViewPort(get_width(), get_height()); 

  // *** OpenGL BEGIN ***

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 
  
  spikeWaveRenderer_.render(); 

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***

  //invalidate(); 
  return true;
}


bool SpikeWaveView::on_expose_event(GdkEventExpose* event)
{


  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  spikeWaveRenderer_.render(); 

  // Swap buffers.
  gldrawable->swap_buffers();
  gldrawable->gl_end();

  frameCount_++;
  
  return true;
}

bool SpikeWaveView::on_map_event(GdkEventAny* event)
{

  //  std::cout << "SpikeWaveView::on_map_event" << std::endl;

  spikeWaveRenderer_.setViewPort(get_width(), get_height()); 
  
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 

  spikeWaveRenderer_.render(); 
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

  spikeWaveRenderer_.setViewPort(get_width(), get_height()); 

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


void SpikeWaveView::invalidate() {
  // invalidate the whole window
  Glib::RefPtr<Gdk::Window> win = get_window();
  Gdk::Rectangle r(0, 0, get_allocation().get_width(),
		   get_allocation().get_height());
  win->invalidate_rect(r, false);
  
}

void SpikeWaveView::setTime(uint64_t ts)
{
  currentTime_ = ts; 
}


bool SpikeWaveView::on_motion_notify_event(GdkEventMotion* event)
{

  float x = event->x;
  float y = event->y;


  if (event->state & GDK_BUTTON1_MASK) 
    {
      
      float viewY1, viewY2; 
      spikeWaveRenderer_.getAmplitudeView(&viewY1, &viewY2); 

      float pixYdelta = -(lastY_ - y); 
      float zoomYfact = pixYdelta/float(get_width()) + 1.0; 
      viewY1 = viewY1 * zoomYfact; 
      viewY2 = viewY2 * zoomYfact;  
      spikeWaveRenderer_.setAmplitudeView(viewY1, viewY2); 

      amplitudeViewChangedSignal_.emit(viewY1, viewY2); 

      invalidate(); 

    }

      
  else if (event->state & GDK_BUTTON3_MASK)
    {

    }

  lastX_ = x; 
  lastY_ = y; 

  return true;
}


bool SpikeWaveView::on_button_press_event(GdkEventButton* event)
{

  float x = event->x;
  float y = event->y;

  lastX_ = event->x; 
  lastY_ = event->y; 
 
  return false;
}

int SpikeWaveView::getFrameCount()
{
  int f = frameCount_; 
  frameCount_ = 0; 
  return f; 

}


void SpikeWaveView::setLive(bool live)
{
  live_ = live; 
  spikeWaveRenderer_.setLive(live); 

}

void SpikeWaveView::updateState(const TSpikeChannelState & state)
{
  float min, max; 

  min = state.rangeMin / 1e9; 
  max = state.rangeMax / 1e9; 

  spikeWaveRenderer_.setAmplitudeRange(min, max); 
  spikeWaveRenderer_.updateState(state); 
}


void SpikeWaveView::resetView()
{
  float min, max;
  spikeWaveRenderer_.getAmplitudeRange(&min, &max); 
  spikeWaveRenderer_.setAmplitudeView(min, max); 

}

void SpikeWaveView::resetData()
{
  spikeWaveRenderer_.resetData();

}
