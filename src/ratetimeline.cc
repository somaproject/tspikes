#include "ratetimeline.h"
#include "glconfig.h"
#include "shaders.h"

RateTimeline::RateTimeline() : 
  majorTick_(1000), 
  minorTick_(100), 
  decayRange_(100), 
  viewLatest_(true), 
  cutoffPos_(0), 
  viewX1_(0.0), 
  viewX2_(10000), 
  viewY1_(0.0), 
  viewY2_(100.0)
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
  add_events(Gdk::VISIBILITY_NOTIFY_MASK);


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

// bool RateTimeline::setViewingWindow(float x1, float y1, 
// 				   float x2, float y2)
// {
//   viewX1_ = x1; 
//   viewY1_ = y1; 
//   viewX2_ = x2; 
//   viewY2_ = y2; 

//   viewChanged_ = true; 
// }

void RateTimeline::updateViewingWindow()

{
  glLoadIdentity(); 

  glOrtho(viewX1_, viewX2_, viewY1_, viewY2_, -3, 3); 

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

void RateTimeline::appendRate(RateVal_t x)
{
  
  rates_.push_back(x); 

  viewX2_ = rates_.size(); 

  if (x > viewY2_) {
    //viewY2_ = x+ 1; 
  }
  // invalidate
  updateViewingWindow(); 
  get_window()->invalidate_rect(get_allocation(), true);
  
}

bool RateTimeline::on_expose_event(GdkEventExpose* event)
{
  std::cout << "expose event" << std::endl; 
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();


  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  glClearColor(0.0, 0.0, 0.0, 1.0); 
  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 

  int N = rates_.size(); 
  
  glLineWidth(1.0); 
  // draw minor ticks
  glColor4f(0.0, 0.0, 1.0, 0.5); 

  for (int i = 0; i < (N / minorTick_ + 1); i++)
    {
      glBegin(GL_LINE_STRIP); 
      glVertex2f(float(i*minorTick_), 0.0); 
      glVertex2f(float(i*minorTick_), viewY2_); 

      glEnd(); 
	
    }

  // draw major ticks
  glColor4f(0.7, 0.7, 1.0, 0.9); 

  for (int i = 0; i < (N / majorTick_ + 1); i++)
    {
      glBegin(GL_LINE_STRIP); 
      glVertex2f(float(i*majorTick_), 0.0); 
      glVertex2f(float(i*majorTick_), viewY2_); 

      glEnd(); 
	
    }

  // draw actual primary line


  useGPUProgram(gpuProgGradient_); 
  // now draw filled segment gradient
  GLint vp = glGetUniformLocation(gpuProgGradient_, "rate"); 
  glUniform1f(vp, float(decayRange_)); 
  GLint vp2 = glGetUniformLocation(gpuProgGradient_, "cutoff"); 
  if (viewLatest_) {
    glUniform1f(vp2, float(rates_.size())); 
  } else {
    glUniform1f(vp2, float(cutoffPos_)); 
  }
	       


  // primary polygons (filled)

  for (int i = 1; i < N ; i++)
    {

      glBegin(GL_POLYGON); 
      float ratel = rates_[i-1]; 
      float rater = rates_[i]; 
      glVertex2f(float(i-1), 0.0);
      glVertex2f(float(i-1), ratel); 
      glVertex2f(float(i), rater); 
      glVertex2f(float(i), 0.0); 
      glEnd(); 
    }

  glEnd(); 
  useGPUProgram(0); 

  // main line
  glColor4f(1.0, 1.0, 1.0, 1.0); 

  glBegin(GL_LINE_STRIP); 
  for (int i = 0; i < N ; i++)
    {
      glVertex2f(float(i), rates_[i]); 
    }
  glEnd(); 
  
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

