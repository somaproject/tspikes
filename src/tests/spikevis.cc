/* Conversion to gtkglextmm by Naofumi Yasufuku */

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

#include "glspikes.h"
#include "ttreader.h"
#include "clusterview.h"


const int N = 400000;
const int M = 6000;  

GLfloat points[N][4]; 

static GLuint color_tex_id; 

struct CRGBA 
{
  GLubyte R; 
  GLubyte G; 
  GLubyte B; 
  GLubyte A; 
} __attribute__((packed)) ; 

//std::vector<SP> SpikePoints; 
//std::vector<CRGBA> SpikeColors; 
GLfloat SpikeColors2[1000000]; 

GLSPVectpList spvl; 

static GLuint fbo_id; 

void checkerr(void)
{
  GLenum e = glGetError(); 
  const GLubyte *errString; 
  while (e != GL_NO_ERROR) {
    errString = gluErrorString(e); 
    printf("THere was an error %d %s\n", e, errString ); 
    e = glGetError(); 
  }
}

//////////////////////////////////////////////////////////////////////////////
//
// OpenGL frame buffer configuration utilities.
//
//////////////////////////////////////////////////////////////////////////////

struct GLConfigUtil
{
  static void print_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig,
                              const char* attrib_str,
                              int attrib,
                              bool is_boolean);

  static void examine_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig);
};

//
// Print a configuration attribute.
//
void GLConfigUtil::print_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig,
                                   const char* attrib_str,
                                   int attrib,
                                   bool is_boolean)
{
  int value;

  if (glconfig->get_attrib(attrib, value))
    {
      //std::cout << attrib_str << " = ";
      //if (is_boolean)
        //std::cout << (value == true ? "true" : "false") << std::endl;
      //else
        //std::cout << value << std::endl;
    }
  else
    {
       std::cout << "*** Cannot get "
                 << attrib_str
                 << " attribute value\n";
    }
}

//
// Print configuration attributes.
//
void GLConfigUtil::examine_gl_attrib(const Glib::RefPtr<const Gdk::GL::Config>& glconfig)
{
  std::cout << "\nOpenGL visual configurations :\n\n";

  std::cout << "glconfig->is_rgba() = "
            << (glconfig->is_rgba() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->is_double_buffered() = "
            << (glconfig->is_double_buffered() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->is_stereo() = "
            << (glconfig->is_stereo() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_alpha() = "
            << (glconfig->has_alpha() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_depth_buffer() = "
            << (glconfig->has_depth_buffer() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_stencil_buffer() = "
            << (glconfig->has_stencil_buffer() ? "true" : "false")
            << std::endl;
  std::cout << "glconfig->has_accum_buffer() = "
            << (glconfig->has_accum_buffer() ? "true" : "false")
            << std::endl;

  std::cout << std::endl;

}


///////////////////////////////////////////////////////////////////////////////
//
// Vis scene.
//
///////////////////////////////////////////////////////////////////////////////

class VisScene : public Gtk::GL::DrawingArea
{
public:
  explicit VisScene(GLSPVectpList * pspvl);
  virtual ~VisScene();

protected:

  // signal handlers:
  virtual void on_realize();
  virtual bool on_configure_event(GdkEventConfigure* event);
  virtual bool on_expose_event(GdkEventExpose* event);
  virtual bool on_map_event(GdkEventAny* event);
  virtual bool on_unmap_event(GdkEventAny* event);
  virtual bool on_visibility_notify_event(GdkEventVisibility* event);
  virtual bool on_idle();
  void resetAccumBuffer(GLSPVectpList::iterator sstart, 
				GLSPVectpList::iterator send); 
  void renderSpikeVector(const GLSPVect * spvect); 

  GLSPVectpList*  pspvl_; 

public:
  // Invalidate whole window.
  void invalidate() {
    get_window()->invalidate_rect(get_allocation(), false);
  }

  // Update window synchronously (fast).
  void update()
  { get_window()->process_updates(false); }


protected:
  // idle signal connection:
  sigc::connection m_ConnectionIdle;

public:
  // get & set view rotation values.

protected:
  // OpenGL scene related variables:

  bool m_IsSync;

protected:
  // frame rate evaluation stuff:
  Glib::Timer m_Timer;
  Glib::Timer m_UpdateTimer; 
  int m_Frames;
  GLSPVectpList::iterator curSPVectpIter_; 

};

VisScene::VisScene(GLSPVectpList * pspvl)
  : pspvl_(pspvl), 
    m_IsSync(true),
    m_Frames(0),
    curSPVectpIter_(pspvl->begin())

{
  //
  // Configure OpenGL-capable visual.
  //

  Glib::RefPtr<Gdk::GL::Config> glconfig;

  // Try double-buffered visual
  glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB    |
                                     Gdk::GL::MODE_DOUBLE | 
				     Gdk::GL::MODE_ACCUM);
  if (!glconfig)
    {
      std::cerr << "*** Cannot find the double-buffered visual.\n"
                << "*** Trying single-buffered visual.\n";

      // Try single-buffered visual
      glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB   |
                                         Gdk::GL::MODE_DEPTH);
      if (!glconfig)
        {
          std::cerr << "*** Cannot find any OpenGL-capable visual.\n";
          std::exit(1);
        }
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

VisScene::~VisScene()
{
}

/*
 * Draw a gear wheel.  You'll probably want to call this function when
 * building a display list since we do a lot of trig here.
 *
 * Input:  inner_radius - radius of hole at center
 * outer_radius - radius at center of teeth
 * width - width of gear
 * teeth - number of teeth
 * tooth_depth - depth of tooth
 */


void VisScene::on_realize()
{
  std::cout << "VisScene::on_realize()" << std::endl; 

  // We need to call the base on_realize()
  Gtk::DrawingArea::on_realize();

  //
  // Get GL::Drawable.
  //

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  //
  // GL calls.
  //

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return;



//   glEnable(GL_BLEND); 
//   glBlendFunc(GL_SRC_ALPHA, GL_ONE); 


  // Make the vis.
  
  //glEnable(GL_NORMALIZE);
  glEnable(GL_POINT_SMOOTH); 
  glEnableClientState(GL_VERTEX_ARRAY); 
  glEnableClientState(GL_COLOR_ARRAY); 
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  gldrawable->gl_end();
  // *** OpenGL END ***

  // Start timer.
  m_Timer.start();
  m_UpdateTimer.start(); 
}

bool VisScene::on_configure_event(GdkEventConfigure* event)
{

  std::cout << "VisScene::on_configure_event()" << std::endl; 

  //
  // Get GL::Drawable.
  //

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  //
  // GL calls.
  //

  // *** OpenGL BEGIN ***

  if (!gldrawable->gl_begin(get_gl_context()))
    return false;
  gldrawable->wait_gdk(); 
  glDrawBuffer(GL_BACK); 

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 3, 0, 3, -3, 3);   
  glViewport(0, 0, get_width(), get_height());
  glClearColor(0.0, 0.0, 0.0, 1.0); 
  glClear(GL_COLOR_BUFFER_BIT | GL_ACCUM_BUFFER_BIT ); 
  


  resetAccumBuffer(pspvl_->begin(), curSPVectpIter_); 
  gldrawable->wait_gl(); 
  //gldrawable->wait_gdk(); 
  
  checkerr(); 
  gldrawable->gl_end();
  // *** OpenGL END ***


  return true;
}

bool VisScene::on_expose_event(GdkEventExpose* event)
{
  //std::cout << "VisScene::on_expose_event()" << std::endl; 

  //
  // Get GL::Drawable.
  //

  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  //
  // GL calls.
  //

  // *** OpenGL BEGIN ***
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  glDrawBuffer(GL_BACK); 
  
  // copy things into current buffer
  glAccum(GL_RETURN, 1.0); 
  if (curSPVectpIter_ != pspvl_->end()) {
    renderSpikeVector(*curSPVectpIter_); 
  }
  
  double secsu = m_UpdateTimer.elapsed(); 
  if (secsu >= 0.01) 
    {
      glAccum(GL_MULT, 0.995); 
      //checkerr(); 

      glClear(GL_COLOR_BUFFER_BIT); 

	if (curSPVectpIter_ != pspvl_->end()) {	
	  renderSpikeVector(*curSPVectpIter_); 
  	  glAccum(GL_ACCUM, 1.0); 

	  ++curSPVectpIter_;
	}
	m_UpdateTimer.reset(); 
	glAccum(GL_RETURN, 1.0); 
    }
  
  // Swap buffers.
  gldrawable->swap_buffers();


  gldrawable->gl_end();
  // *** OpenGL END ***

  //
  // Print frame rate.
  //


  ++m_Frames;

  double seconds = m_Timer.elapsed();
  
  if (seconds >= 5.0)
    {
      std::cout << " there were " << pspvl_->size() << " spike vectors" 
		<< std::endl; 
      // std::cout.setf(std::ios_base::fixed, std::ios_base::floatfield);
      std::cout.setf(std::ios::fixed, std::ios::floatfield);
      std::cout.precision(3);
      std::cout << m_Frames << " frames in "
                << seconds << " seconds = "
                << (m_Frames / seconds) << " FPS\n";
      m_Timer.reset();
      m_Frames = 0;
  int bits; 
  glGetIntegerv(GL_ACCUM_BLUE_BITS, &bits); 
  std::cout << "There are " << bits << "Accum red bits" << std::endl; 

    }


  return true;
}

bool VisScene::on_map_event(GdkEventAny* event)
{
  std::cout << "map event" << std::endl; 
  
  if (!m_ConnectionIdle.connected())
    m_ConnectionIdle = Glib::signal_idle().connect(
      sigc::mem_fun(*this, &VisScene::on_idle), GDK_PRIORITY_REDRAW);

  return true;
}

bool VisScene::on_unmap_event(GdkEventAny* event)
{
  if (m_ConnectionIdle.connected())
    m_ConnectionIdle.disconnect();

  return true;
}

bool VisScene::on_visibility_notify_event(GdkEventVisibility* event)
{
  std::cout << "visibility notify" << std::endl; 
  if (event->state == GDK_VISIBILITY_FULLY_OBSCURED)
    {
      if (m_ConnectionIdle.connected())
        m_ConnectionIdle.disconnect();
    }
  else
    {
      if (!m_ConnectionIdle.connected())
        m_ConnectionIdle = Glib::signal_idle().connect(
          sigc::mem_fun(*this, &VisScene::on_idle), GDK_PRIORITY_REDRAW);
    }
  resetAccumBuffer(spvl.begin(), curSPVectpIter_); 

  return true;
}

bool VisScene::on_idle()
{

  // Invalidate the whole window.
  invalidate();

  // Update window synchronously (fast).
//   if (m_IsSync)
  update();

  return true;
}


void VisScene::renderSpikeVector(const GLSPVect * spvect)
{
  // take the spikes in the SPvect and render them on the current
  // buffer; we assume viewport and whatnot are already configured
  

  glColor3f(1.0, 1.0, 0.0); 
  glVertexPointer(2, GL_FLOAT, sizeof(GLSpikePoint),
		  &((*spvect)[0])); 
  std::vector<CRGBA> colors(spvect->size()); 
  for(int i = 0; i < spvect->size(); i++)
    {
      CRGBA c; 
      int tchan = (*spvect)[i].tchan; 
      switch (tchan) 
	{
	case 0:
	  c.R = 255; 
	  c.G = 0;
	  c.B = 0; 
	  c.A = 0; 
	  break; 

	case 1:
	  c.R = 0; 
	  c.G = 255; 
	  c.B = 0; 
	  c.A = 0; 
	  break; 

	case 2:
	  c.R = 0; 
	  c.G = 0; 
	  c.B = 255; 
	  c.A = 0; 
	  break; 

	case 3:
	  c.R = 255; 
	  c.G = 255; 
	  c.B = 0; 
	  c.A = 0; 
	  break; 
	default:
	  break;
	}
      colors[i] = c; 
    }
  glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(CRGBA), 
		 &colors[0]); 
  glDrawArrays(GL_POINTS, 0, spvect->size()); 


}

void VisScene::resetAccumBuffer(GLSPVectpList::iterator sstart, 
			      GLSPVectpList::iterator send)
{
  // first clear accumulation buffer
  glClearAccum(0.0, 0.0, 0.0, 0.0); 
  glClearColor(0.0, 0.0, 0.0, 0.0); 
  
  glClear(GL_COLOR_BUFFER_BIT |  GL_ACCUM_BUFFER_BIT); 
  
  GLSPVectpList::iterator i; 
  glReadBuffer(GL_BACK); 
  for (i = sstart; i != send; i++)
    {
      renderSpikeVector(*i); 
      glAccum(GL_ACCUM, 1.0); 
      glAccum(GL_MULT, 0.99); 
      glClear(GL_COLOR_BUFFER_BIT); 
	
    }


}
///////////////////////////////////////////////////////////////////////////////
//
// The application class.
//
///////////////////////////////////////////////////////////////////////////////

class Vis : public Gtk::Window
{
public:
  explicit Vis(bool is_sync = true);
  virtual ~Vis();

protected:
  // signal handlers:
  void on_button_quit_clicked();
  virtual bool on_key_press_event(GdkEventKey* event);

protected:
  // member widgets:
  Gtk::VBox m_VBox;
  VisScene m_VisScene;
  Gtk::Button m_ButtonQuit;
};

Vis::Vis(bool is_sync)
  : m_VBox(false, 0), 
    m_VisScene(&spvl), 
    m_ButtonQuit("Quit")
{
  //
  // Top-level window.
  //

  set_title("Vis");

  // Get automatically redrawn if any of their children changed allocation.
  set_reallocate_redraws(true);

  add(m_VBox);

  //
  // Vis scene.
  //

  m_VisScene.set_size_request(200, 200);

  m_VBox.pack_start(m_VisScene);

  //
  // Simple quit button.
  //

  m_ButtonQuit.signal_clicked().connect(
    sigc::mem_fun(*this, &Vis::on_button_quit_clicked));

  m_VBox.pack_start(m_ButtonQuit, Gtk::PACK_SHRINK, 0);

  //
  // Show window.
  //

  show_all();
}

Vis::~Vis()
{
}

void Vis::on_button_quit_clicked()
{
  Gtk::Main::quit();
}

bool Vis::on_key_press_event(GdkEventKey* event)
{

  m_VisScene.invalidate();


  return true;
}


void spikesquares(void)
{
  
  float scale = 0.0035; 
  const int MAXX = 30; 
  const int MAXY = 30; 
  
  const int SPACEX = 40; 
  const int SPACEY = 40; 
  
  for (int i = 0; i < 20; i++)
    { 
      for (int j = 0 ; j < 20; j++)
	{
	  GLSPVect * spv = new GLSPVect; 
	  
	  for (int x = 0; x < MAXX; x++) 
	    { 
	      for (int y = 0; y < MAXY; y++)
		{
		  GLSpikePoint sp1; 


		  sp1.p1 = float( x + j * SPACEX) * scale; 
		  sp1.p2 = float( y + i * SPACEY) * scale; 
		  sp1.p3 = 0.0; //float( x + i * SPACEX) * scale; 
		  sp1.p4 = 0.0; //float( y + i * SPACEY) * scale; 
		  sp1.ts = 1000; 
		  sp1.tchan = j % 4; 
		  spv->push_back(sp1); 
		}
	    }
	  spvl.push_back(spv); 

	}
    }
}
///////////////////////////////////////////////////////////////////////////////
//
// Main.
//
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
  Gtk::Main kit(argc, argv);

  //
  // Init gtkglextmm.
  //

  Gtk::GL::init(argc, argv);
  spikesquares(); 
  
  Vis vis(true);

  kit.run(vis);

  return 0;
}
