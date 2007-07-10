#include "ratetimeline.h"
#include "glconfig.h"
#include "shaders.h"

RateTimeline::RateTimeline() : 
  BASEDURATION(1000.0), 
  majorTick_(BASEDURATION), 
  minorTick_(BASEDURATION/10.0), 
  viewT1_(0.0), 
  viewT2_(BASEDURATION), 
  viewX1_(0.0), 
  viewX2_(100.0),
  isLive_(true), 
  decayRate_(0.01), 
  activePos_(100.0)
{

  rates_.reserve(10000); 

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

  // Add events.
  add_events(Gdk::BUTTON1_MOTION_MASK    |
	     Gdk::BUTTON2_MOTION_MASK    |
	     Gdk::BUTTON3_MOTION_MASK | 
	     Gdk::SCROLL_MASK |
	     Gdk::BUTTON_PRESS_MASK      |
	     Gdk::VISIBILITY_NOTIFY_MASK);
  
  // View transformation signals.
  signal_button_press_event().connect(sigc::mem_fun(*this, 
						    &RateTimeline::on_button_press_event)); 
  
  signal_motion_notify_event().connect(sigc::mem_fun(*this, 
						     &RateTimeline::on_motion_notify_event)); 

  signal_scroll_event().connect(sigc::mem_fun(*this, 
					      &RateTimeline::on_scroll_event)); 

}

RateTimeline::~RateTimeline()
{
}


void RateTimeline::on_realize()
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

  GLuint vshdr = loadGPUShader("rtgrad.vert", GL_VERTEX_SHADER); 
  GLuint fshdr = loadGPUShader("rtgrad.frag", GL_FRAGMENT_SHADER); 
  std::list<GLuint> shaders; 
  shaders.push_back(vshdr); 
  shaders.push_back(fshdr); 
  gpuProgGradient_ = createGPUProgram(shaders); 

  updateViewingWindow(); 

  gldrawable->gl_end();
  // *** OpenGL END ***

}

void RateTimeline::updateViewingWindow()

{
  glLoadIdentity(); 

  glOrtho(viewT1_, viewT2_, viewX1_, viewX2_, -3, 3); 

  glViewport(0, 0, get_width(), get_height());
  //viewChanged_ = false; 
  
}

bool RateTimeline::on_configure_event(GdkEventConfigure* event)
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

void RateTimeline::appendRate(RatePoint_t rp)
{
  if (rates_.empty() ){
    viewT1_ = rp.time; 
  }

  rates_.push_back(rp); 

  //viewT2_ = rates_.size(); 
  if (isLive_) {
    activePos_ = rp.time; 
    if (rp.time > viewT2_ ) {
      float width = viewT2_ - viewT1_; 
      viewT2_ = rp.time ; 
      viewT1_ = viewT2_ - width; 
    }
  }
  updateViewingWindow(); 
  invalidate(); 

}


void RateTimeline::drawTicks()
{

  int N = rates_.size(); 
  


  glLineWidth(1.0); 
  // draw minor ticks
  glColor4f(0.0, 0.0, 1.0, 0.5); 
  
  for (int i = 0; i*minorTick_ < viewT2_ ; i++)
    {
      glBegin(GL_LINE_STRIP); 
      glVertex2f(float(i*minorTick_), viewX1_); 
      glVertex2f(float(i*minorTick_), viewX2_); 

      glEnd(); 
	
    }

  // draw major ticks
  glColor4f(0.7, 0.7, 1.0, 0.9); 

  for (int i = 0; i * majorTick_ <= viewT2_; i++)
    {
      glBegin(GL_LINE_STRIP); 
      glVertex2f(float(i*majorTick_), viewX1_); 
      glVertex2f(float(i*majorTick_), viewX2_); 

      glEnd(); 
	
    }

  // draw actual primary line
}
bool RateTimeline::on_expose_event(GdkEventExpose* event)
{

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  glClearColor(0.0, 0.0, 0.0, 1.0); 
  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  updateViewingWindow(); 

  drawTicks(); 
  
  useGPUProgram(gpuProgGradient_); 
  // now draw filled segment gradient
  GLint vp = glGetUniformLocation(gpuProgGradient_, "decayRate"); 
  glUniform1f(vp, decayRate_); 
  GLint vp2 = glGetUniformLocation(gpuProgGradient_, "activePos"); 
  glUniform1f(vp2, activePos_); 


  // primary polygons (filled)

  for (int i = 1; i < rates_.size() ; i++)
    {
      
       glBegin(GL_POLYGON); 
       timeval_t timel = rates_[i-1].time; 
       timeval_t timeh = rates_[i].time; 
       rateval_t ratel = rates_[i-1].rate; 
       rateval_t rater = rates_[i].rate; 
       glVertex2f(timel, 0.0);
       glVertex2f(timel, ratel); 
       glVertex2f(timeh, rater); 
       glVertex2f(timeh, 0.0); 
       glEnd(); 
     }

  glEnd(); 
   
   useGPUProgram(0); 

  // main line
  glColor4f(1.0, 1.0, 1.0, 1.0); 

  glBegin(GL_LINE_STRIP); 
  for (int i = 0; i < rates_.size() ; i++)
    {
      glVertex2f(rates_[i].time, rates_[i].rate); 
    }

  glEnd(); 

//   //active Line
//   glLineWidth(4.0); 
//   glColor4f(0.0, 1.0, 0.0, 1.0); 
//   glBegin(GL_LINE_STRIP); 
//   glVertex2f(float(N-1), viewX1_); 
//   glVertex2f(float(N-1), viewX2_); 
//   glEnd(); 

  // Swap buffers.
  gldrawable->swap_buffers();
  gldrawable->gl_end();

  return true;
}

bool RateTimeline::on_map_event(GdkEventAny* event)
{

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 

  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  
  // set zoom to be correct
  int pixnum = get_width(); 
  
  updateViewingWindow(); 

  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  gldrawable->gl_end();
  // *** OpenGL END ***


  return true;
}

bool RateTimeline::on_unmap_event(GdkEventAny* event)
{

  return true;
}

bool RateTimeline::on_visibility_notify_event(GdkEventVisibility* event)
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

bool RateTimeline::on_button_press_event(GdkEventButton* event)
{

  float x = event->x;
  float y = event->y;

  if (event->button == 3)
    {
      float windowPos = float(x) / get_width() * (viewT2_ - viewT1_); 

      activePos_ = windowPos + viewT1_; 

      invalidate(); 
      viewSignal_.emit(isLive_, activePos_, decayRate_); 

      
      update();
    }
  
  lastX_ = event->x; 
 
  return false;
}

bool RateTimeline::on_motion_notify_event(GdkEventMotion* event)
{

  float x = event->x;
  float y = event->y;

  if (event->state & GDK_BUTTON1_MASK) 
    {
      
      float movePixDelta = float(x) - float(lastX_); 
      
      // how many GL units was this? 
      float windowDelta = movePixDelta / get_width() * (viewT2_ - viewT1_); 
      
      viewT1_ -= windowDelta; 
      viewT2_ -= windowDelta;
      
      invalidate(); 
      
      update();
    } 
  else if (event->state & GDK_BUTTON3_MASK)
    {

      float windowPos = float(x) / get_width() * (viewT2_ - viewT1_); 

      activePos_ = viewT1_ + windowPos; 
      invalidate(); 

      viewSignal_.emit(isLive_, activePos_, decayRate_); 
      update();
    }
  lastX_ = x; 
  // don't block
  return true;
}

bool RateTimeline::on_scroll_event(GdkEventScroll* event)
{

  float x = event->x;
  float y = event->y;
  
  if (event->state & GDK_BUTTON3_MASK){
    
    if (event->direction == 0) {
      // UP? zoom out
      decayRate_ = decayRate_ * 1.1; 
      
    } else {
      decayRate_ = decayRate_ / 1.1; 
      
     }
    viewSignal_.emit(isLive_, activePos_, decayRate_); 
	  
    
    invalidate();
    
    update();

  } 
  else 
    {
      
      float centerPos = (x / get_width()) * (viewT2_ - viewT1_)  +  viewT1_; 
      
      float newzoom = 1.0; 
      
      if (event->direction == 0) {
	// UP? zoom out
	//newzoom = zoomLevel_ *  1.41421356237;
	
      } else {
	//newzoom = zoomLevel_ / 1.41421356237; 
	
      }
      
      //setZoom(newzoom, centerPos); 
      
      invalidate(); 
      
      update();
    }
  return false; 
}

RateTimeline::viewsignal_t RateTimeline::viewSignal()
{
  return viewSignal_;
  
}

void RateTimeline::setLive(bool v)
{
  isLive_ = v; 
  invalidate(); 

}
